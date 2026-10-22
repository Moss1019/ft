//
// Created by moss on 09-10-2026.
//

#include "LandmarkDetector.h"

#include <opencv2/imgproc.hpp>

LandmarkDetector::LandmarkDetector(const Ort::Env &env,
                                   const Ort::SessionOptions &options,
                                   Ort::MemoryInfo *memInfo,
                                   const std::string &modelPath,
                                   int size)
    : m_memInfo(memInfo), m_size(size)
{
    m_session = new Ort::Session(env, modelPath.c_str(), options);
    m_allocator = new Ort::AllocatorWithDefaultOptions();

    m_inputNames[0] = "image";

    m_outputNames[0] = "scores";
    m_outputNames[1] = "landmarks";

    m_inputShape[0] = 1;
    m_inputShape[1] = 3;
    m_inputShape[2] = 192;
    m_inputShape[3] = 192;

    m_landmarkBuffer = new float[468 * 3];

    m_buffer = new float[3 * 192 * 192];

    m_channels[0] = cv::Mat(size, size, CV_32F, m_buffer);
    m_channels[1] = cv::Mat(size, size, CV_32F, m_buffer + size * size);
    m_channels[2] = cv::Mat(size, size, CV_32F, m_buffer + 2 * size * size);
}

LandmarkDetector::~LandmarkDetector()
{
    delete m_session;
    delete m_allocator;
    delete[] m_buffer;
    delete[] m_landmarkBuffer;
}

bool LandmarkDetector::detect(const cv::Mat &croppedFrame)
{
    if (!m_session || !m_memInfo)
    {
        return false;
    }
    cv::Mat rgb;
    cv::cvtColor(croppedFrame, rgb, cv::COLOR_BGR2RGB);
    cv::Mat floatImg;
    rgb.convertTo(floatImg, CV_32F, 1.0f / 255.0f);
    cv::split(floatImg, m_channels);
    const Ort::Value inputTensor = Ort::Value::CreateTensor<float>(*m_memInfo,
                                                                   m_buffer,
                                                                   3 * m_size * m_size,
                                                                   m_inputShape.data(),
                                                                   m_inputShape.size());
    const auto output = m_session->Run(Ort::RunOptions{nullptr},
                                               m_inputNames.data(),
                                               &inputTensor,
                                               m_inputNames.size(),
                                               m_outputNames.data(),
                                               m_outputNames.size());
    const auto scoreCount = output[0].GetTensorSizeInBytes() / sizeof(float);
    if (scoreCount == 0)
    {
        return false;
    }
    const auto scores = output[0].GetTensorData<float>();
    const auto score = scores[0];
    std::memcpy(m_landmarkBuffer, output[1].GetTensorData<float>(), output[1].GetTensorSizeInBytes());
    return true;
}

Vec2 LandmarkDetector::getVec2(int index) const
{
    return Vec2(m_landmarkBuffer + index * 3);
}

Vec3 LandmarkDetector::getVec3(int index) const
{
    return Vec3(m_landmarkBuffer + index * 3);
}
