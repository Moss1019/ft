#pragma once
#include <string>

#include "onnxruntime_cxx_api.h"

class OnnxModelDetails
{
public:
    [[nodiscard]]
    static std::string getInputInfo(Ort::Session &session, Ort::AllocatorWithDefaultOptions &allocator);

    [[nodiscard]]
    static std::string getOutputInfo(Ort::Session &session, Ort::AllocatorWithDefaultOptions &allocator);
private:
    [[nodiscard]]
    static std::string shapeStr(const std::vector<int64_t> &shape);

    [[nodiscard]]
    static std::string elemTypeStr(ONNXTensorElementDataType type);
};
