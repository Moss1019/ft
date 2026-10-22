#pragma once

#include <opencv2/core/mat.hpp>

#include "onnxruntime_cxx_api.h"

struct Vec2
{
public:
    explicit Vec2(float *buffer): m_buffer(buffer) {}

    [[nodiscard]]
    float x() const
    {
        return m_buffer[0];
    }

    [[nodiscard]]
    float y() const
    {
        return m_buffer[1];
    }
private:
    float *m_buffer;
};

struct Vec3
{
public:
    explicit Vec3(float *buffer): m_buffer(buffer) {}

    [[nodiscard]]
    float x() const
    {
        return m_buffer[0];
    }

    [[nodiscard]]
    float y() const
    {
        return m_buffer[1];
    }

    [[nodiscard]]
    float z() const
    {
        return m_buffer[2];
    }
private:
    float *m_buffer;
};

class LandmarkDetector
{
public:
    explicit LandmarkDetector(const Ort::Env &env,
        const Ort::SessionOptions &options,
        Ort::MemoryInfo *memInfo,
        const std::string &modelPath,
        int size = 192);

    ~LandmarkDetector();

    [[nodiscard]]
    bool detect(const cv::Mat &croppedFrame);

    [[nodiscard]]
    Vec2 getVec2(int index) const;

    [[nodiscard]]
    Vec3 getVec3(int index) const;

    [[nodiscard]]
    static size_t landmarkCount() { return 468; }
private:
    Ort::MemoryInfo *m_memInfo;

    std::vector<const char *> m_inputNames{1};

    std::vector<const char *> m_outputNames{2};

    std::vector<int64_t> m_inputShape{{0, 0, 0, 0}};

    Ort::Session *m_session = nullptr;

    Ort::AllocatorWithDefaultOptions *m_allocator = nullptr;

    std::vector<cv::Mat> m_channels{3};

    float *m_buffer = nullptr;

    int m_size = 0;

    float *m_landmarkBuffer = nullptr;
};
