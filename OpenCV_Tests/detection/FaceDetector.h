#pragma once

#include <opencv2/core/mat.hpp>

#include "onnxruntime_cxx_api.h"

class FaceDetector
{
public:
    explicit FaceDetector(const Ort::Env &env,
        const Ort::SessionOptions &options,
        Ort::MemoryInfo *memInfo,
        const std::string &modelPath,
        int size = 320);

    ~FaceDetector();

    [[nodiscard]]
    int size() const { return m_size; }

    [[nodiscard]]
    bool detect(const cv::Mat &frame,
        float threshold,
        float nmsThreshold,
        float eta,
        cv::Mat &res,
        cv::Rect2f &cropBox);

    [[nodiscard]]
    std::string getInputInfo() const;

    [[nodiscard]]
    std::string getOutputInfo() const;
private:
    Ort::MemoryInfo *m_memInfo;

    std::vector<const char *> m_inputNames{1};

    std::vector<const char *> m_outputNames{12};

    std::vector<int64_t> m_inputShape{{0, 0, 0, 0}};

    Ort::Session *m_session = nullptr;

    Ort::AllocatorWithDefaultOptions *m_allocator = nullptr;

    std::vector<cv::Mat> m_channels{3};

    float *m_buffer = nullptr;

    int m_size = 0;
};

struct LandmarkDetails
{
    int c;
    int r;
    int s;
    const float *buffer;
};
