# matmul_improved 优化实现计划

## 目标

在 Apple M5 (ARM64) 上实现高性能矩阵乘法，逐层叠加优化，最终接近 OpenBLAS 性能。

## 当前基准 (matmul_plain)

| 矩阵大小 | 耗时 | 性能 |
|---------|------|------|
| 128×128 | 0.003s | 1.4 GFLOPS |
| 1024×1024 | 1.1s | 1.9 GFLOPS |
| 8192×8192 | — | 太慢跳过 |

M5 单核理论峰值约 100+ GFLOPS，plain 仅利用 ~2%。

---

## 第一层：算法层优化 ✅ 已完成

### 1.1 循环重排 (i, k, j)

**原理：** 将内层循环变量从 j（导致 B 按列跳跃访问，stride=n）改为让 j 在最内层，使 B 和 C 都按行连续访问（stride=1）。

```c
// 优化前 (i,j,k)：B 按列访问 ❌
for (i) for (j) for (k)
    C[i][j] += A[i][k] * B[k][j];  // B: stride = n

// 优化后 (i,k,j)：B 按行访问 ✅
for (i) for (k) {
    a_ik = A[i][k];                  // 标量，存寄存器
    for (j)
        C[i][j] += a_ik * B[k][j];  // B: stride = 1, C: stride = 1
}
```

### 1.2 分块 Tiling (64x64)

**原理：** 将大矩阵切成 64x64 的小块，让 3 个子块（A_tile, B_tile, C_tile）总共 ~48 KB，能完全驻留在 M5 的 128 KB L1 数据缓存中。

**结构：** 6 层循环 = 外 3 层遍历 tile + 内 3 层计算 tile 内乘法。

```c
// 外三层：遍历 tile（块）
for (ii += TILE) for (kk += TILE) for (jj += TILE)
    // 内三层：tile 内计算，封装为 tile_multiply()
    tile_multiply(A_sub, B_sub, C_sub, ...);
```

**封装：** 将块内计算抽取为 `static inline void tile_multiply()`，后续只需修改此函数即可加入 SIMD。

### 第一层结果

| 矩阵大小 | plain | improved (第一层) | 加速比 |
|---------|-------|------------------|--------|
| 128×128 | 1.4 GFLOPS | 11.4 GFLOPS | **8x** |
| 1024×1024 | 1.9 GFLOPS | 21.6 GFLOPS | **11x** |
| 8192×8192 | — | 16.4 GFLOPS (67s) | — |

---

## M5 硬件参数（实测）

> 以下数据来自 `sysctl` 实测，针对 MacBook Air M5（Mac17,4）。

| 参数 | 超大核 (P-core) | 能效核 (E-core) | 说明 |
|------|-----------------|-----------------|------|
| L1 数据缓存 | **128 KB / 核** | **64 KB / 核** | 每核独享 |
| L1 指令缓存 | 192 KB / 核 | 128 KB / 核 | 每核独享 |
| L2 缓存 | **16 MB / 集群** | **6 MB / 集群** | 集群内共享 |
| Cache line 大小 | **128 字节** | **128 字节** | = 32 个 float |
| 核数 | 4 | 6 | 共 10 核 |
| 关联度 (ways) | 未公开 | 未公开 | Apple 未披露，推测 8~16 way |

> **注意：M5 的 cache line 是 128 字节（32 个 float），不是 x86 常见的 64 字节。**
> 这影响对齐策略和 SIMD 展开粒度。

---

## 第二层：数据布局优化

> 将数据在内存中重新排列，让后续的计算（无论是标量还是 SIMD）都能更高效地访问。
> 这一层不改变计算逻辑，只改变数据的摆放方式。

### 2.1 矩阵 B Packing

**原理：** 在计算前，将 B 的每个 tile 提前复制并重排到连续缓冲区，消除 cache set 冲突。

**目前的问题：** `tile_multiply` 中 B 的访问模式 `B[ki * ldb + j]`——行内连续（j 循环，stride=1 ✅），但行间跳跃（ki 循环，stride=ldb）。

B tile 的每一行在内存中的间距是 `ldb`（= n，完整矩阵宽度），不是 `tile_n`（块宽度）。每行加载的 cache line 本身没有浪费（只加载用到的部分），但 **行间的大跨步导致 cache set 冲突**：

```
tile_multiply 中 B 的访问顺序：

ki=0:  B[0 * ldb + 0..63]      ← 地址偏移 0
ki=1:  B[1 * ldb + 0..63]      ← 地址偏移 1024×4 = 4096 字节
ki=2:  B[2 * ldb + 0..63]      ← 地址偏移 8192 字节
...
ki=63: B[63 * ldb + 0..63]     ← 地址偏移 258048 字节
```

**Cache set 冲突原理：**

CPU cache 按 set 组织，每个地址通过取模映射到固定的 set：
```
set index = (地址 / cache_line_size) % set总数
```

当 `ldb` 是 2 的幂（如 1024）时，行间距 = 4096 字节，恰好是 cache 结构的整数倍。
多行映射到同一个 set → set 内位置有限（N-way） → 行数超过 N 时互相淘汰。

**后果：** 外层 `i` 循环每次迭代都要重新扫 B 的全部 64 行（ki=0..63）。
如果 B 的行在 cache 中互相淘汰，则 `i=0` 时加载的 B 数据在 `i=1` 时已被踢出，
每次 `i` 迭代都产生 cache miss → **B tile 数据被反复从内存加载 64 次，而非只加载 1 次**。

```
B tile（64×64）需要 16 KB 数据
但散布在 64 × ldb × 4 = 256 KB 的地址范围内（当 n=1024）
                                 ↑ 超过 L1d 的 128 KB
```

**Packing 后（紧凑排列）：**

```
packed_B（行间距 = tile_n = 64 floats = 256 字节）：

ki=0:  地址 0      → set A
ki=1:  地址 256    → set B    ← 不同 set！
ki=2:  地址 512    → set C    ← 不同 set！
...

16 KB 数据在 16 KB 连续地址范围内 → 各行映射到不同 set → 无冲突
→ i=0 加载一次，i=1..63 全部 cache 命中
```

**做法：** 在外层循环中，调用 `tile_multiply` 前将 B tile 拷贝到紧凑缓冲区：

```c
// 打包：将 B[kk..kk+tile_k][jj..jj+tile_n] 复制到连续内存
float packed_B[TILE_SIZE * TILE_SIZE];  // 紧凑存储

for (size_t ki = 0; ki < tile_k; ki++) {
    memcpy(&packed_B[ki * tile_n],         // 目标：间距 = tile_n
           &B[(kk+ki) * n + jj],           // 源：间距 = n
           tile_n * sizeof(float));
}

// 计算时用 packed_B（间距变成 tile_n，更紧凑）
tile_multiply(A_sub, packed_B, C_sub, lda, tile_n, ldc, ...);
//                                          ↑ ldb 从 n 变成 tile_n
```

> **数论视角：** 冲突的根源是 `gcd(stride_in_cache_lines, set总数) > 1`。
> Packing 让 stride 变小，使 gcd 趋近 1（各行均匀分散到不同 set）。
> 另一种轻量方案是 **Padding**——每行多分配几个 float，使 stride 与 set 数量互素，无需拷贝。

**预期效果：** ~10-30% 提升（大矩阵更明显，因为 n 越大 set 冲突越严重）

### 2.2 内存对齐

**原理：** 让矩阵数据起始地址对齐到 **128 字节**（M5 的 cache line 大小），加载更高效，且为后续 SIMD 对齐加载做准备。

**改动：** 修改 `matrix_create()` 中的内存分配：

```c
// 原来
mat->data = (float *)calloc(rows * cols, sizeof(float));

// 改为对齐分配（128 字节 = M5 cache line 大小）
mat->data = (float *)aligned_alloc(128, rows * cols * sizeof(float));
memset(mat->data, 0, rows * cols * sizeof(float));
```

**预期效果：** ~10-20% 额外提升

### 2.3 A 矩阵 Packing (Micro-Panel 打包)

**原理：** 与 B Packing 对称，将 A 的 tile 子块也重排到连续内存。当 micro-kernel 同时读取 A 的多行（见 3.2 寄存器 Blocking）时，A 的行间距 `lda = k` 同样引发 cache set 冲突。

**目前的问题：** micro-kernel 需要同时读取 A 的 `mr` 行（如 8 行）来填充寄存器累加器。这些行在原始矩阵中间距为 `lda * 4` 字节，当 `lda` 是 2 的幂时，多行映射到同一 cache set，产生与 B 相同的冲突淘汰问题。

**做法：** 将 A tile 重排为按 `mr` 行一组的连续条带（panel），每组内按列优先存储，使 micro-kernel 可以连续加载 A 的多行元素：

```c
// 打包 A[ii..ii+tile_m][kk..kk+tile_k] 为 micro-panel 格式
// 按 mr 行为一组，组内按列优先（column-major within panel）
void pack_A_panel(const float *src, float *dst,
                  size_t src_ld, size_t tile_m, size_t tile_k, size_t mr) {
    for (size_t i = 0; i < tile_m; i += mr) {
        size_t actual_mr = (i + mr <= tile_m) ? mr : (tile_m - i);
        for (size_t ki = 0; ki < tile_k; ki++) {
            for (size_t ir = 0; ir < actual_mr; ir++)
                *dst++ = src[(i + ir) * src_ld + ki];
            for (size_t ir = actual_mr; ir < mr; ir++)
                *dst++ = 0.0f;  // 不足 mr 行时填零
        }
    }
}

// 打包后的内存布局（mr=8 为例）：
// [a00 a10 a20 a30 a40 a50 a60 a70] [a01 a11 a21 ... a71] ...
//  ^-- 第 0 列的 8 个元素连续           ^-- 第 1 列的 8 个元素连续
```

> **与 B Packing 的配合：** A pack 为 `mr * kc` panel，B pack 为 `kc * nr` panel。
> micro-kernel 从 packed A 连续读 `mr` 个元素，从 packed B 连续读 `nr` 个元素，
> 累加到寄存器中的 `mr * nr` 个 C 元素。三者配合实现 **全寄存器计算** -- 所有 load 都是连续地址，无 cache 冲突。

**预期效果：** 配合寄存器 blocking micro-kernel，~10-20% 额外提升

---

## 第三层：硬件特性利用

> 利用 CPU 硬件特性：SIMD 向量指令、寄存器文件深度、预取机制、多级缓存层次、多核并行。
> 3.1-3.2 改变 `tile_multiply` 内部的计算方式，3.3-3.5 进一步压榨硬件能力。

### 3.1 ARM NEON SIMD

**原理：** 使用 ARM NEON 128-bit 向量寄存器，一条指令同时处理 4 个 float。

**核心改动：** 修改 `tile_multiply()` 的内层循环，将 j 的步长从 1 改为 4。

```c
#include <arm_neon.h>

// tile_multiply 内层循环改造
for (size_t i = 0; i < tile_m; i++) {
    for (size_t ki = 0; ki < tile_k; ki++) {
        // 广播 A[i][ki] 到 4 个通道
        float32x4_t va = vdupq_n_f32(A[i * lda + ki]);

        size_t j = 0;
        // NEON 主循环：每次处理 4 个 float
        for (; j + 4 <= tile_n; j += 4) {
            float32x4_t vb = vld1q_f32(&B[ki * ldb + j]);     // 加载 B 4个元素
            float32x4_t vc = vld1q_f32(&C[i * ldc + j]);      // 加载 C 4个元素
            vc = vfmaq_f32(vc, va, vb);                        // C += A * B (FMA)
            vst1q_f32(&C[i * ldc + j], vc);                    // 写回 C
        }
        // 标量尾部：处理剩余 0~3 个元素
        for (; j < tile_n; j++) {
            C[i * ldc + j] += A[i * lda + ki] * B[ki * ldb + j];
        }
    }
}
```

**关键指令：**

| 指令 | 功能 | 说明 |
|------|------|------|
| `vdupq_n_f32(s)` | 广播标量 | `[s, s, s, s]` |
| `vld1q_f32(ptr)` | 加载 4 个 float | 从连续地址读取 |
| `vfmaq_f32(c,a,b)` | 融合乘加 | `c += a * b`，一条指令完成乘+加 |
| `vst1q_f32(ptr, v)` | 存储 4 个 float | 写回连续地址 |

**进一步优化 -- Loop Unrolling：** 每次处理 16 个 float（4 组 NEON 寄存器），提升指令级并行（ILP），让 CPU 流水线中 load/store 和 FMA 指令交错执行。

```c
// 展开到 16 个 float/次
for (; j + 16 <= tile_n; j += 16) {
    vc0 = vfmaq_f32(vc0, va, vld1q_f32(&B[...+j]));
    vc1 = vfmaq_f32(vc1, va, vld1q_f32(&B[...+j+4]));
    vc2 = vfmaq_f32(vc2, va, vld1q_f32(&B[...+j+8]));
    vc3 = vfmaq_f32(vc3, va, vld1q_f32(&B[...+j+12]));
    // 写回 4 组
}
```

**预期效果：** ~2-3x 提升（相对于标量 tiling）

### 3.2 寄存器 Blocking (Micro-Kernel)

**原理：** 3.1 的简单 NEON 向量化只利用了 SIMD 的"宽度"（一次算 4 个 float），但没有利用"深度" -- M5 有 **32 个 128-bit NEON 寄存器**（v0~v31）。寄存器 blocking 通过在寄存器中同时驻留 C 子块的 **多行累加器**，将计算/访存比从 O(1) 提升到 O(mr)。

**这是从 ~30 GFLOPS 到 80+ GFLOPS 的关键一步**，也是 OpenBLAS、BLIS 等高性能库的核心技术。

**Micro-kernel 尺寸选择：** `mr * nr = 8 * 16`

- C 累加器：8 行 * 4 个 `float32x4_t` = **32 个寄存器**（刚好用满 v0~v31）
- 每次 ki 迭代：加载 A 的 8 个标量（广播）+ B 的 16 个 float（4 组 NEON），执行 32 次 FMA
- 32 次 FMA = 256 FLOP，但只需从 L1 加载 24 个 float, 计算密度极高

```c
#define MR 8
#define NR 16

// 8x16 micro-kernel: C[8][16] += A[8][kc] * B[kc][16]
static inline void micro_kernel_8x16(
    const float *packed_A, const float *packed_B,
    float *C, size_t ldc, size_t kc) {

    // C 累加器：32 个 NEON 寄存器
    float32x4_t vc[8][4];
    for (int i = 0; i < 8; i++)
        for (int r = 0; r < 4; r++)
            vc[i][r] = vld1q_f32(&C[i * ldc + r * 4]);

    // 主循环：沿 k 维度累加
    for (size_t ki = 0; ki < kc; ki++) {
        float32x4_t vb0 = vld1q_f32(&packed_B[ki * NR + 0]);
        float32x4_t vb1 = vld1q_f32(&packed_B[ki * NR + 4]);
        float32x4_t vb2 = vld1q_f32(&packed_B[ki * NR + 8]);
        float32x4_t vb3 = vld1q_f32(&packed_B[ki * NR + 12]);

        for (int i = 0; i < 8; i++) {
            float32x4_t va = vdupq_n_f32(packed_A[ki * MR + i]);
            vc[i][0] = vfmaq_f32(vc[i][0], va, vb0);
            vc[i][1] = vfmaq_f32(vc[i][1], va, vb1);
            vc[i][2] = vfmaq_f32(vc[i][2], va, vb2);
            vc[i][3] = vfmaq_f32(vc[i][3], va, vb3);
        }
    }

    // 写回 C
    for (int i = 0; i < 8; i++)
        for (int r = 0; r < 4; r++)
            vst1q_f32(&C[i * ldc + r * 4], vc[i][r]);
}
```

**计算密度对比：**

| 方案 | 每次 ki 的 FLOP | 每次 ki 的 load | 计算/访存比 |
|------|----------------|----------------|-------------|
| 3.1 简单 NEON (1*n) | 2*tile_n | tile_n + tile_n | 1:1 |
| 3.2 Micro-kernel (8*16) | 256 | 8+16 = 24 | **10.7:1** |

> **为什么 8*16?** 8*16 = 128 个 C 累加器 = 32 个 NEON 寄存器，刚好用满 AArch64 的 v0~v31。
> 更大的 micro-kernel（如 12*16）会导致寄存器溢出到栈（register spilling），反而变慢。

**预期效果：** 在 3.1 基础上再 ~2-3x, 单核 60-90 GFLOPS

### 3.3 Software Prefetch (软件预取)

**原理：** M5 有硬件预取器，对顺序访问模式效果很好，但在首次访问新 tile 或跨步访问时响应较慢。软件预取通过 `__builtin_prefetch` 提前发出 cache line 加载请求，让数据在需要时已在 L1 中就绪。

**适用场景：**

1. **micro-kernel 内部** -- 提前 2-3 轮预取后续 ki 对应的 packed B 数据：

```c
for (size_t ki = 0; ki < kc; ki++) {
    // 预取 ki+2 轮要用的 B 数据（提前 2 轮，给 L1 加载留出时间）
    __builtin_prefetch(&packed_B[(ki + 2) * NR], 0, 3);  // 0=读, 3=最高局部性

    // ... FMA 计算 ...
}
```

2. **tile 循环间** -- 提前预取即将处理的下一个 tile 首块数据，减少"冷启动" miss：

```c
for (size_t jj = 0; jj < n; jj += TILE_SIZE) {
    if (jj + TILE_SIZE < n)
        __builtin_prefetch(&data_b[kk * n + jj + TILE_SIZE], 0, 1);
    // ... pack + compute ...
}
```

> **注意：** 过多 prefetch 会占用 load 端口、污染 cache。建议在 NEON + micro-kernel 完成后，
> 通过 benchmark 对比有无 prefetch 的性能差异，按需保留。

**预期效果：** ~5-10%（最后阶段的微调手段）

### 3.4 分层 Tiling (L1 + L2 两级分块)

**原理：** 当前单一 `TILE_SIZE=64` 是为标量计算设计的（3*16KB=48KB < 128KB L1）。加入 NEON micro-kernel 后，计算速度大幅提升，数据消耗更快 -- tile 太小则切换开销占比增大，太大则溢出 L1。最优策略是引入 **两级 tiling**，分别匹配 L1 和 L2 的容量。

**BLIS 风格的三层参数：**

| 参数 | 含义 | 推荐值 (M5) | 对应缓存 |
|------|------|-------------|----------|
| `mc` | A panel 行数 | 128 | A panel (mc*kc) 驻留 L2 |
| `kc` | 共享维度块大小 | 128 | B panel (kc*nc) 驻留 L2 |
| `nc` | B panel 列数 | 512 | -- |
| `mr` | micro-kernel 行数 | 8 | C micro-tile 驻留寄存器 |
| `nr` | micro-kernel 列数 | 16 | C micro-tile 驻留寄存器 |

**循环结构（5 层 + micro-kernel）：**

```c
// L2 级 tiling
for (jc = 0; jc < n; jc += nc)        // B panel 列
  for (pc = 0; pc < k; pc += kc) {    // 共享维度
    pack_B(B, packed_B, pc, jc, kc, nc);  // 整个 B panel 打包一次

    for (ic = 0; ic < m; ic += mc) {  // A panel 行
      pack_A(A, packed_A, ic, pc, mc, kc);  // A panel 打包一次

      // micro-kernel 级：遍历 panel 内的 mr*nr 小块
      for (jr = 0; jr < nc; jr += NR)
        for (ir = 0; ir < mc; ir += MR)
          micro_kernel_8x16(packed_A + ir*kc,
                            packed_B + jr*kc,
                            &C[(ic+ir)*n + jc+jr], n, kc);
    }
  }
```

**与单层 tiling 的区别：**

- **单层（当前）：** 一个 64*64 tile 同时服务于 A、B、C，tile 间频繁切换
- **两级：** L2 级的大 tile（mc*kc、kc*nc）减少 packing 次数，L1 级的 micro-kernel（mr*nr）在寄存器中完成计算
- packed B panel 在 L2 中被 `mc/mr` 个 micro-kernel 行复用，**B 的 L2 命中率大幅提升**

> **调参建议：** `mc*kc*4` 字节应 <= L1d 的一半（~64 KB），`kc*nc*4` 字节应 <= L2 的一部分（~4 MB）。
> 具体值需要 benchmark 微调，上表为推荐起点。

**预期效果：** ~10-15% 提升（大矩阵更明显）

### 3.5 OpenMP 多线程

**原理：** 将外层行循环分配到 M5 的多个性能核并行执行。

**核心改动：** 在最外层 `ii` 循环前加 `#pragma omp parallel for`。

```c
#pragma omp parallel for schedule(static)
for (size_t ii = 0; ii < m; ii += TILE_SIZE) {
    // 每个线程负责几个行 tile
    for (size_t kk = ...) {
        for (size_t jj = ...) {
            tile_multiply(...);
        }
    }
}
```

**编译选项：**

```bash
# macOS 需要先安装 libomp
brew install libomp

# 编译
gcc -O3 -Xpreprocessor -fopenmp -I$(brew --prefix libomp)/include \
    -L$(brew --prefix libomp)/lib -lomp \
    main.c matrix.c matmul_plain.c matmul_improved.c -lm -o matmul_test
```

**预期效果：** 在 NEON 基础上再 ~4-8x -> 100-200 GFLOPS

---

## 三层优化的关系

三层优化在概念上独立，但第二层和第三层在实施时存在 **协同设计** 关系：

| 层 | 改什么 | 涉及代码 | 与其他层的关系 |
|---|--------|---------|---------------|
| 第一层（算法） | 循环顺序 + 分块结构 | 外层循环框架 | 独立，提供基础框架 |
| 第二层（数据布局） | 数据在内存中的排列 | packing 函数 + `matrix_create` | A/B packing 格式需匹配第三层 micro-kernel 尺寸 |
| 第三层（硬件） | 指令 + 寄存器利用 + 预取 + 缓存层次 | micro-kernel + tiling 参数 | micro-kernel 的 mr*nr 决定第二层 packing 布局 |

> **协同关系说明：** 第二层的 A/B packing 布局由第三层 micro-kernel 的 `mr * nr` 尺寸决定。
> 因此实施时建议：先确定 micro-kernel 尺寸（3.2），再据此设计 packing 格式（2.3），最后调整 tiling 参数（3.4）。

---

## 预期性能总结

| 阶段 | 技术 | 预期 GFLOPS | vs plain |
|------|------|------------|----------|
| 基准 | matmul_plain (i,j,k) | ~2 | 1x |
| 第一层 ✅ | 循环重排 + Tiling | ~16-21 | 8-11x |
| 第二层 ✅ | + B Packing + 内存对齐 | ~25-28 | 12-14x |
| 第二层 | + A Packing | ~28-33 | 14-17x |
| 第三层 | + NEON 向量化（简单） | ~30-40 | 15-20x |
| 第三层 | + 寄存器 Blocking (8*16) | ~60-90 | 30-45x |
| 第三层 | + Prefetch + 分层 Tiling | ~70-100 | 35-50x |
| 第三层 | + OpenMP 多线程 | ~150-300 | 75-150x |
| 对比 | OpenBLAS | ~100-200 | -- |

## 文件结构

```
code/
├── matrix.h              # 结构体定义 + 函数声明
├── matrix.c              # 工具函数（create/free/random/equal/print）
├── matmul_plain.c        # 朴素实现（基准）
├── matmul_improved.c     # 优化实现（所有优化集成在此文件）
└── main.c                # 测试驱动 + benchmark 对比
```

## 实施顺序

1. [x] 第一层：循环重排 + Tiling
2. [x] 第二层：B Packing + 内存对齐
3. [ ] 第二层：A Packing（micro-panel 格式，需配合步骤 5 的 mr 尺寸）
4. [ ] 第三层：NEON 向量化（简单版，修改 `tile_multiply`）
5. [ ] 第三层：寄存器 Blocking（8*16 micro-kernel）
6. [ ] 第三层：Software Prefetch（micro-kernel 内 + tile 间）
7. [ ] 第三层：分层 Tiling（L1+L2 两级，调整 mc/kc/nc 参数）
8. [ ] 第三层：OpenMP 多线程（外层循环并行）
9. [ ] OpenBLAS 对比测试
10. [ ] 完整 benchmark（16, 128, 1K, 8K, 64K）
