# OpenCV DNN 动态量化 — Instructor 讨论提纲

## 我做了什么

在 OpenCV DNN 模块中实现了 Dynamic Quantization 支持，修改了 8 个文件：

- 新增两个层：`QuantizeDynamicLayer`（运行时通过 `minMaxIdx` 计算 scale/zp）和 `DequantizeDynamicLayer`
- 扩展了 `Net::quantize()` API 增加 `dynamicQuant` 参数
- 修改了网络图构建逻辑，动态模式下插入动态量化节点
- 在 ONNX importer 中添加了 `DynamicQuantizeLinear` 算子支持

代码编译通过，单元测试不影响现有功能。

---

## 什么是"图"和"图变换"

DNN 里的"图"就是网络的计算结构。每个层是一个节点，数据流是边：

```
原始图:      _input(FP32) → Conv(FP32) → 输出
                               ↓ quantize() 做图变换
量化后的图:  _input(FP32) → [Quantize] → ConvInt8 → [Dequantize] → 输出
```

"图变换" = 遍历图的边，在 FP32↔INT8 的转换点自动插入 Quantize/Dequantize 节点。

---

## 发现的历史问题

### PR [#24980](https://github.com/opencv/opencv/pull/24980)（2024年2月，已 merge 到 5.x）

**标题**: "dnn cleanup: On-fly-quantization removal"

**作者说的**: 
> "on-fly-quantization is less practical given the fact that there has been so many dedicated tools for model quantization"

**做了什么**: 删除了 `Net::quantize()` API 和 `net_quantization.cpp` 中的图变换逻辑。**保留了** int8 层实现（用于加载预量化 ONNX 模型）。

### 这跟我的实现的关系

| | 被删除的方案 | 我的路径 A | 我的路径 B |
|---|---|---|---|
| 入口 | `Net::quantize()` | `Net::quantize(dynamic=true)` | `readNetFromONNX()` |
| 做什么 | OpenCV 做图变换插入静态量化节点 | OpenCV 做图变换插入动态量化节点 | 加载已含 DynamicQuantizeLinear 的模型 |
| 官方态度 | ❌ 5.x 已删除 | ❓ 本质上同一种方法 | ✅ 官方保留了 int8 层就是为了这个 |

**路径 A 和被删方案本质一样** — 都是 OpenCV 自己做图变换来量化。差异仅在于 scale 是固定值还是每帧计算。

**路径 B 是独立的** — 不涉及图变换，纯粹让 OpenCV 能推理由 PyTorch/ONNX Runtime 导出的动态量化模型。

---

## 待讨论的问题

1. **方向选择**: 应该走路径 A（扩展 `quantize()` API）还是路径 B（只做 ONNX import 支持）？
   - 如果只做路径 B，可以删掉对 `net_quantization.cpp`、`net.cpp`、`dnn.hpp` 的改动
   - 只保留层实现 + 注册 + ONNX import

2. **跟 5.x 分支的关系**: 我们的代码基于 4.x，但 5.x 已经删掉了 `Net::quantize()`。我们的改动是否需要考虑向前兼容？

3. **定位**: 这个项目的目标是学习 OpenCV 内部架构和量化原理，还是产出可以合并到上游的代码？这会影响设计决策。
