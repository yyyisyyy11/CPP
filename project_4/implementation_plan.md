# 为 OpenCV DNN 加入 Dynamic Quantization 支持

## 背景

OpenCV DNN 目前只支持 **Static Quantization**（静态量化），即 scale/zeropoint 在 `Net::quantize()` 校准或 ONNX 模型导入时就固定了。本方案旨在增加 **Dynamic Quantization**（动态量化）模式，在推理时根据输入数据实时计算激活值的 scale/zeropoint，与现有的静态量化共存。

### 静态 vs 动态量化对比

| 特性 | 静态量化 (现有) | 动态量化 (新增) |
|------|----------------|----------------|
| Scale/ZP 计算时机 | 校准阶段一次性计算 | 每次 forward 时实时计算 |
| 权重量化 | ✅ 提前量化 | ✅ 提前量化 (与静态相同) |
| 激活值量化 | 固定 scale | 动态计算 scale |
| 精度 | 依赖校准数据代表性 | 更高 (适应每个输入) |
| 性能 | 更快 (无运行时统计开销) | 稍慢 (需要 minMaxIdx) |

---

## Proposed Changes

### Component 1: 公共头文件 — 新增层声明

#### [MODIFY] [all_layers.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/include/opencv2/dnn/all_layers.hpp)

在现有 `QuantizeLayer`（L465）和 `DequantizeLayer`（L473）声明之后，新增：

```cpp
class CV_EXPORTS QuantizeDynamicLayer : public QuantizeLayer
{
public:
    bool perChannel;  // 是否 per-channel 动态量化
    static Ptr<QuantizeDynamicLayer> create(const LayerParams &params);
};

class CV_EXPORTS DequantizeDynamicLayer : public DequantizeLayer
{
public:
    static Ptr<DequantizeDynamicLayer> create(const LayerParams &params);
};
```

继承 `QuantizeLayer` / `DequantizeLayer` 以复用 `scales`/`zeropoints` 成员。

---

#### [MODIFY] [dnn.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/include/opencv2/dnn/dnn.hpp)

扩展 `Net::quantize()` 签名（L668），添加 `dynamicQuant` 参数：

```cpp
CV_WRAP Net quantize(InputArrayOfArrays calibData, int inputsDtype, int outputsDtype, 
                     bool perChannel=true, bool dynamicQuant=false);
```

- `dynamicQuant=false`（默认）：现有静态量化逻辑不变
- `dynamicQuant=true`：权重静态量化，激活值通过 `QuantizeDynamic` 层在运行时动态量化

---

### Component 2: 动态量化层实现

#### [MODIFY] [quantization_utils.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/int8layers/quantization_utils.cpp)

在文件末尾的工厂函数（L580）之前，新增两个类。

##### QuantizeDynamicLayerImpl

```cpp
// Dynamic Quantize: compute scale/zp at runtime from activation min/max
class QuantizeDynamicLayerImpl CV_FINAL : public QuantizeDynamicLayer
{
public:
    int axis;
    bool perChannel;

    QuantizeDynamicLayerImpl(const LayerParams& params)
    {
        axis = params.get<int>("axis", 1);
        perChannel = params.get<bool>("per_channel", false);
        setParamsFrom(params);
    }

    bool supportBackend(int backendId) CV_OVERRIDE
    {
        return backendId == DNN_BACKEND_OPENCV;
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() == 1);
        outputs.resize(3);
        outputs[0] = inputs[0];          // quantized INT8 data (same shape)
        outputs[1] = MatShape({1, 1});   // scale  (scalar or 1-D)
        outputs[2] = MatShape({1, 1});   // zeropoint (scalar or 1-D)
        return false;
    }

    void forward(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr,
                 OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        const int qmin = -128, qmax = 127;
        double rmin, rmax;
        cv::minMaxIdx(inputs[0], &rmin, &rmax);
        rmin = std::min(rmin, 0.0);
        rmax = std::max(rmax, 0.0);

        float sc = (float)((rmax == rmin) ? 1.0 : (rmax - rmin) / (qmax - qmin));
        int zp = (int)std::round(qmin - rmin / sc);

        // Store for getScaleZeropoint() queries
        scales.resize(1);    scales[0] = sc;
        zeropoints.resize(1); zeropoints[0] = zp;

        // output[0]: quantized data
        inputs[0].convertTo(outputs[0], CV_8S, 1.0 / sc, zp);

        // output[1]: scale, output[2]: zeropoint — for downstream DequantizeDynamic
        outputs[1].create(1, 1, CV_32F);
        outputs[1].at<float>(0) = sc;
        outputs[2].create(1, 1, CV_32F);
        outputs[2].at<float>(0) = (float)zp;
    }
};
```

##### DequantizeDynamicLayerImpl

```cpp
// Dynamic Dequantize: reads scale/zp from input tensors (produced by QuantizeDynamic)
class DequantizeDynamicLayerImpl CV_FINAL : public DequantizeDynamicLayer
{
public:
    DequantizeDynamicLayerImpl(const LayerParams& params)
    {
        setParamsFrom(params);
    }

    bool supportBackend(int backendId) CV_OVERRIDE
    {
        return backendId == DNN_BACKEND_OPENCV;
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        // inputs[0]=INT8 data, inputs[1]=scale, inputs[2]=zeropoint
        CV_Check(inputs.size(), inputs.size() >= 1 && inputs.size() <= 3, "");
        outputs.assign(1, inputs[0]);
        return false;
    }

    void forward(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr,
                 OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        float sc = 1.0f;
        int zp = 0;
        if (inputs.size() > 1)
        {
            sc = inputs[1].at<float>(0);
            if (inputs.size() > 2)
                zp = (int)inputs[2].at<float>(0);
        }

        inputs[0].convertTo(outputs[0], CV_32F, sc, -(sc * zp));
    }
};
```

新增工厂函数（在 L580-L593 区域之后）：

```cpp
Ptr<QuantizeDynamicLayer> QuantizeDynamicLayer::create(const LayerParams& params)
{
    return Ptr<QuantizeDynamicLayer>(new QuantizeDynamicLayerImpl(params));
}

Ptr<DequantizeDynamicLayer> DequantizeDynamicLayer::create(const LayerParams& params)
{
    return Ptr<DequantizeDynamicLayer>(new DequantizeDynamicLayerImpl(params));
}
```

---

### Component 3: 多输出 tensor 机制 — 详解

> [!NOTE]
> **为什么需要额外输出？** 这是用户反馈需要解释的核心设计。

问题：`QuantizeDynamic` 在 `forward()` 中动态算出 `scale=0.05, zp=-3`，但后面的 `DequantizeDynamic` 也需要同样的值来反量化。**层之间传递数据的唯一方式就是 tensor 连线。**

以一个简单的网络为例，完整的图结构如下：

```
              ┌─ output[0]: INT8 数据 ──→ [ConvInt8] ──→ INT8 结果 ──→ [DequantizeDynamic] input[0]
FP32 输入 → [QuantizeDynamic] ├─ output[1]: scale (1×1 Mat) ──────────────────→ [DequantizeDynamic] input[1]
              └─ output[2]: zp (1×1 Mat) ─────────────────────→ [DequantizeDynamic] input[2]
```

对应的网络构建代码（在 `net_quantization.cpp` 中）：
```cpp
// 1. 添加 QuantizeDynamic 层 (3个输出)
int qLid = dstNet.addLayer("quantize_dyn/input", "QuantizeDynamic", CV_8S, lp);

// 2. 连接: QuantizeDynamic.output[0] → ConvInt8.input[0]  (INT8数据)
dstNet.connect(qLid, 0, convLid, 0);

// 3. 后续添加 DequantizeDynamic 层
int dqLid = dstNet.addLayer("dequantize_dyn/conv", "DequantizeDynamic", CV_32F, lp);

// 4. 连接: ConvInt8.output[0] → DequantizeDynamic.input[0]  (INT8结果)
dstNet.connect(convLid, 0, dqLid, 0);

// 5. 连接: QuantizeDynamic.output[1] → DequantizeDynamic.input[1]  (scale)
dstNet.connect(qLid, 1, dqLid, 1);

// 6. 连接: QuantizeDynamic.output[2] → DequantizeDynamic.input[2]  (zeropoint)
dstNet.connect(qLid, 2, dqLid, 2);
```

这与 ONNX 的 `QuantizeLinear(x, scale, zp)` / `DequantizeLinear(x, scale, zp)` 语义完全一致 — scale 和 zp 都是图中显式的边，不是隐藏状态。

**现有的 `DequantizeLayerImpl` 已经支持这种用法**（[quantization_utils.cpp L383-L410](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/int8layers/quantization_utils.cpp#L383-L410)）：`inputs.size() > 1` 时就从输入 tensor 读取 scale/zp。我们的 `DequantizeDynamicLayerImpl` 采用相同模式。

---

### Component 4: Per-Channel 动态量化策略

> [!NOTE]
> **建议：默认 per-tensor，通过参数可选 per-channel**

Per-channel 动态量化（对每个 channel 分别算 min/max）在精度上更好，但开销更大。我的建议是：

**采用与静态量化一致的策略** — 用 `per_channel` 参数控制：
- `per_channel=false`（默认）：对整个 tensor 做一次 `minMaxIdx`，一个 scale/zp
- `per_channel=true`：对每个 output channel 分别算 min/max

实际场景中：
- **Conv/FC 的权重**：per-channel 静态量化（已有），精度关键
- **激活值**：per-tensor 动态量化就够了，因为同一层的激活值分布通常差异不大

所以默认 `per_channel=false` 是合理的。如果用户对某些特殊模型需要更高精度，可以设为 `true`。

在 `QuantizeDynamicLayerImpl::forward()` 中的 per-channel 实现：

```cpp
if (perChannel && inputs[0].dims >= 2)
{
    int channels = inputs[0].size[axis];
    scales.resize(channels);
    zeropoints.resize(channels);
    
    // 对每个 channel 分别统计
    for (int c = 0; c < channels; c++)
    {
        // 提取 channel slice, 计算 min/max
        // ... (使用 axis 参数确定切片维度)
    }
    // 逐 channel 量化
}
else
{
    // per-tensor: 一次 minMaxIdx 搞定 (默认路径)
    // ... (如上面的代码)
}
```

---

### Component 5: 层注册

#### [MODIFY] [init.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/init.cpp)

在 L204-L206 现有的 `Quantize`/`Dequantize` 注册之后添加：

```cpp
CV_DNN_REGISTER_LAYER_CLASS(QuantizeDynamic,   QuantizeDynamicLayer);
CV_DNN_REGISTER_LAYER_CLASS(DequantizeDynamic, DequantizeDynamicLayer);
```

---

### Component 6: 动态量化网络构建逻辑

#### [MODIFY] [net_quantization.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net_quantization.cpp)

**1. 修改函数签名**（L36），添加 `dynamicQuant` 参数：

```cpp
Net Net::Impl::quantize(Net& net, InputArrayOfArrays calibData, 
                        int inputsDtype, int outputsDtype, 
                        bool perChannel, bool dynamicQuant)
```

**2. 动态模式下简化校准**（L77-L126 区域）：

动态量化仍然需要一次 forward 来确定权重的 scale/zp（因为权重仍然静态量化），但激活值的 scale/zp 改为占位值：

```cpp
if (dynamicQuant)
{
    // 激活值的 scale/zp 用占位值，运行时由 QuantizeDynamic 动态计算
    sc.push_back(1.0f);   // placeholder
    zp.push_back(0);      // placeholder
}
else
{
    // 现有逻辑：从输出 blob 统计 min/max
    getQuantizationParams(ld.outputBlobs[i], sc, zp);
}
```

**3. 在 Quantize/Dequantize 节点插入处**（L219-L253），根据 `dynamicQuant` 选择类型：

```cpp
if (dynamicQuant)
{
    lp.type = (inpLd.dtype == CV_32F && ld.dtype == CV_8S) 
              ? "QuantizeDynamic" : "DequantizeDynamic";
    
    // 动态模式下额外连接 scale/zp 输出
    if (lp.type == "QuantizeDynamic")
    {
        int qLid = dstNet.addLayer(lp.name, lp.type, ld.dtype, lp);
        dstNet.connect(pin.lid, pin.oid, qLid, 0);
        // 记住 qLid, 后面创建 DequantizeDynamic 时连接 output[1] 和 output[2]
    }
}
else
{
    // 现有逻辑：使用静态 Quantize / Dequantize
    lp.type = (inpLd.dtype == CV_32F && ld.dtype == CV_8S) ? "Quantize" : "Dequantize";
}
```

---

#### [MODIFY] [net_impl.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net_impl.hpp)

更新 `quantize()` 声明（L286）：

```cpp
Net quantize(Net& net, InputArrayOfArrays calibData, int inputsDtype, int outputsDtype, 
             bool perChannel, bool dynamicQuant=false) /*const*/;
```

---

#### [MODIFY] [net.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net.cpp)

更新 `Net::quantize()` 的 wrapper，传递 `dynamicQuant` 参数到 `Impl::quantize()`。

---

### Component 7: ONNX 导入已动态量化模型的支持

#### [MODIFY] [onnx_importer.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/onnx/onnx_importer.cpp)

ONNX 动态量化的模型特征：`QuantizeLinear` 的 scale 输入**不是常量**（不在 `constBlobs` 中），而是另一个计算节点的输出。

现有 `parseQuantDequant()`（L3331）已经处理了这种情况：

```cpp
// L3346-3349: 如果 scale 不是常量，直接 addLayer 并保留图连线
if (constBlobs.find(node_proto.input(1)) == constBlobs.end()) {
    addLayer(layerParams, node_proto);  // scale 作为动态输入
    return;
}
```

但当前这会创建普通的 `Quantize` 层。需要修改为：

```cpp
if (constBlobs.find(node_proto.input(1)) == constBlobs.end()) {
    // Scale is not constant → this is dynamic quantization
    layerParams.type = (node_proto.op_type() == "QuantizeLinear") 
                       ? "QuantizeDynamic" : "DequantizeDynamic";
    addLayer(layerParams, node_proto);
    return;
}
```

同时在 `ifInt8Output()`（L724）的列表中添加 `"QuantizeDynamic"`。

**还需要处理 ONNX Runtime 导出的 `DynamicQuantizeLinear` 算子**（opset 11）：

```cpp
// 新增 parseQuantDequant 中的处理：
dispatch["DynamicQuantizeLinear"] = &ONNXImporter::parseDynamicQuantizeLinear;
```

`DynamicQuantizeLinear` 有 1 个输入（FP32 data）和 3 个输出（UINT8 data, scale, zeropoint），与我们的 `QuantizeDynamicLayer` 设计完全吻合。

---

## 文件修改总结

| 文件 | 操作 | 内容 |
|------|------|------|
| [all_layers.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/include/opencv2/dnn/all_layers.hpp) | MODIFY | 新增 `QuantizeDynamicLayer`, `DequantizeDynamicLayer` 声明 |
| [dnn.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/include/opencv2/dnn/dnn.hpp) | MODIFY | `Net::quantize()` 新增 `dynamicQuant` 参数 |
| [quantization_utils.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/int8layers/quantization_utils.cpp) | MODIFY | 新增两个 Impl 类 + 工厂函数 |
| [init.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/init.cpp) | MODIFY | 注册新层类型 |
| [net_quantization.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net_quantization.cpp) | MODIFY | 动态量化网络构建逻辑 |
| [net_impl.hpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net_impl.hpp) | MODIFY | 更新 `quantize()` 声明 |
| [net.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/net.cpp) | MODIFY | 更新转发调用 |
| [onnx_importer.cpp](file:///Users/mac/Documents/GitHub/opencv/modules/dnn/src/onnx/onnx_importer.cpp) | MODIFY | 支持 ONNX `DynamicQuantizeLinear` + 非常量 scale 场景 |

---

## Verification Plan

### Automated Tests

1. **编译测试**：
   ```bash
   cd /Users/mac/Documents/GitHub/opencv/build
   cmake --build . --target opencv_dnn 2>&1 | tail -20
   ```

2. **功能测试** — `modules/dnn/test/` 中新增：
   - 加载 FP32 模型 → `quantize(calibData, CV_32F, CV_32F, true, true)` → 验证动态量化网络输出
   - 对比动态量化 vs 静态量化的精度差异
   - 验证多次推理时 scale/zp 随输入变化
   - 加载含 `DynamicQuantizeLinear` 的 ONNX 模型 → 验证正确识别

3. **回归测试**：
   ```bash
   ./bin/opencv_test_dnn --gtest_filter="*Quantiz*"
   ```
