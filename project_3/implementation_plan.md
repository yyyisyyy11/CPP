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

## 第二层：数据布局优化

> 将数据在内存中重新排列，让后续的计算（无论是标量还是 SIMD）都能更高效地访问。
> 这一层不改变计算逻辑，只改变数据的摆放方式。

### 2.1 矩阵 B Packing

**原理：** 在计算前，将 B 的每个 tile 提前复制并重排到连续缓冲区，消除跨步问题。

**目前的问题：** 虽然 tile 内 B 按行访问是连续的，但不同行之间的间距是 `ldb`（完整矩阵宽度），不是 `tile_n`（块宽度）。这意味着每行只用了 cache line 的一部分，其余部分是本 tile 不需要的数据。

```
完整 B 矩阵（n=1024），tile_n=64：

行0: [用到的64个] [不需要的960个]    <-- cache 加载了但浪费
行1: [用到的64个] [不需要的960个]
...

打包后 packed_B（紧凑排列）:
行0: [64个]
行1: [64个]   <-- 紧密相邻，cache line 100% 有效
...
```

**做法：** 计算前将 B 的 tile 复制到紧凑缓冲区：

```c
// 打包：将 B[kk..kk+tile_k][jj..jj+tile_n] 复制到连续内存
float packed_B[TILE_SIZE * TILE_SIZE];  // 紧凑存储

for (size_t ki = 0; ki < tile_k; ki++) {
    memcpy(&packed_B[ki * tile_n],         // 目标：紧凑排列
           &B[(kk+ki) * n + jj],           // 源：间距为 n
           tile_n * sizeof(float));
}

// 计算时用 packed_B（间距变成 tile_n，更紧凑）
tile_multiply(A_sub, packed_B, C_sub, lda, tile_n, ldc, ...);
//                                          ↑ ldb 从 n 变成 tile_n
```

**预期效果：** ~10-30% 提升（大矩阵更明显）

### 2.2 内存对齐

**原理：** 让矩阵数据起始地址对齐到 64 字节（cache line 大小），加载更高效，且为后续 SIMD 对齐加载做准备。

**改动：** 修改 `matrix_create()` 中的内存分配：

```c
// 原来
mat->data = (float *)calloc(rows * cols, sizeof(float));

// 改为对齐分配
mat->data = (float *)aligned_alloc(64, rows * cols * sizeof(float));
memset(mat->data, 0, rows * cols * sizeof(float));
```

**预期效果：** ~10-20% 额外提升

---

## 第三层：硬件并行

> 利用 CPU 硬件特性：SIMD 宽指令 + 多核并行。
> 不改变算法逻辑和数据布局，只改变"用什么指令执行计算"。

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

**预期效果：** ~4x 提升 -> 60-80 GFLOPS

### 3.2 OpenMP 多线程

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

## 三层优化的独立性

三层优化彼此独立，互不影响：

| 层 | 改什么 | 涉及代码 | 对其他层的影响 |
|---|--------|---------|---------------|
| 第一层（算法） | 循环顺序 + 分块结构 | 外层 6 重循环框架 | 无 |
| 第二层（数据布局） | 数据在内存中的排列 | packing 函数 + `matrix_create` | 无（只是 `ldb` 变小） |
| 第三层（硬件） | 用什么指令执行计算 | `tile_multiply` 内层循环体 | 无（循环结构不变） |

---

## 预期性能总结

| 阶段 | 技术 | 预期 GFLOPS | vs plain |
|------|------|------------|----------|
| 基准 | matmul_plain (i,j,k) | ~2 | 1x |
| 第一层 ✅ | 循环重排 + Tiling | ~16-21 | 8-11x |
| 第二层 | + B Packing + 内存对齐 | ~20-28 | 10-14x |
| 第三层 | + NEON SIMD | ~60-80 | 30-40x |
| 第三层 | + OpenMP 多线程 | ~100-200 | 50-100x |
| 对比 | OpenBLAS | ~100-200 | — |

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
2. [ ] 第二层：B Packing + 内存对齐
3. [ ] 第三层：NEON SIMD（修改 `tile_multiply`）
4. [ ] 第三层：OpenMP（外层循环并行）
5. [ ] OpenBLAS 对比测试
6. [ ] 完整 benchmark（16, 128, 1K, 8K, 64K）
