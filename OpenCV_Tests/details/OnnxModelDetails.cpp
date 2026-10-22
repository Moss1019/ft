//
// Created by moss on 08-10-2026.
//

#include "OnnxModelDetails.h"

#include <sstream>

std::string OnnxModelDetails::getInputInfo(Ort::Session &session, Ort::AllocatorWithDefaultOptions &allocator)
{
    std::stringstream ss;
    const size_t count = session.GetInputCount();
    ss << "Input count " << count << "\n";
    for (size_t i = 0; i < count; ++i)
    {
        const auto nameAlloc = session.GetInputNameAllocated(i, allocator);
        Ort::TypeInfo typeInfo = session.GetInputTypeInfo(i);
        const auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = tensorInfo.GetShape();
        const auto elemType = tensorInfo.GetElementType();

        ss << "[" << i << "] name: \"" << nameAlloc.get() << "\"";
        ss << " shape: " << shapeStr(shape);
        ss << " data type: " << elemTypeStr(elemType) << "\n";
    }
    return ss.str();
}

std::string OnnxModelDetails::getOutputInfo(Ort::Session &session, Ort::AllocatorWithDefaultOptions &allocator)
{
    std::stringstream ss;
    const size_t count = session.GetOutputCount();
    ss << "Output count " << count << "\n";
    for (size_t i = 0; i < count; ++i)
    {
        const auto nameAlloc = session.GetOutputNameAllocated(i, allocator);
        Ort::TypeInfo typeInfo = session.GetOutputTypeInfo(i);
        const auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();
        std::vector<int64_t> shape = tensorInfo.GetShape();
        const auto elemType = tensorInfo.GetElementType();

        ss << "[" << i << "] name: \"" << nameAlloc.get() << "\"";
        ss << " shape: " << shapeStr(shape);
        ss << " data type: " << elemTypeStr(elemType) << "\n";
    }
    return ss.str();
}

std::string OnnxModelDetails::shapeStr(const std::vector<int64_t> &shape)
{
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < shape.size(); ++i)
    {
        ss << ((shape[i] < 0) ? "dynamic" : std::to_string(shape[i]));
        if (i + 1 < shape.size()) ss << ", ";
    }
    ss << "]";
    return ss.str();
}

std::string OnnxModelDetails::elemTypeStr(ONNXTensorElementDataType type)
{
    switch (type)
    {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT: return "float32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE: return "float64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32: return "int32";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64: return "int64";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8: return "uint8";
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL: return "bool";
        default: return "other(" + std::to_string(static_cast<int>(type)) + ")";
    }
}
