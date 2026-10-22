//
// Created by moss on 08-10-2026.
//

#include "FaceDetector.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "../details/OnnxModelDetails.h"

FaceDetector::FaceDetector(const Ort::Env &env,
                           const Ort::SessionOptions &options,
                           Ort::MemoryInfo *memInfo,
                           const std::string &modelPath,
                           int size)
        :m_memInfo(memInfo), m_size(size)
{
    m_session = new Ort::Session(env, modelPath.c_str(), options);
    m_allocator = new Ort::AllocatorWithDefaultOptions();

    m_inputNames[0] = "input";

    m_outputNames[0] = "cls_8";
    m_outputNames[1] = "cls_16";
    m_outputNames[2] = "cls_32";
    m_outputNames[3] = "obj_8";
    m_outputNames[4] = "obj_16";
    m_outputNames[5] = "obj_32";
    m_outputNames[6] = "bbox_8";
    m_outputNames[7] = "bbox_16";
    m_outputNames[8] = "bbox_32";
    m_outputNames[9] = "kps_8";
    m_outputNames[10] = "kps_16";
    m_outputNames[11] = "kps_32";

    m_inputShape[0] = 1;
    m_inputShape[1] = 3;
    m_inputShape[2] = size;
    m_inputShape[3] = size;

    m_buffer = new float[3 * size * size];
    m_channels[0] = cv::Mat(size, size, CV_32F, m_buffer);
    m_channels[1] = cv::Mat(size, size, CV_32F, m_buffer + size * size);
    m_channels[2] = cv::Mat(size, size, CV_32F, m_buffer + 2 * size * size);
}

FaceDetector::~FaceDetector()
{
    delete m_session;
    delete[] m_buffer;
    delete m_allocator;
}

bool FaceDetector::detect(const cv::Mat &frame,
    float threshold,
    float nmsThreshold,
    float eta,
    cv::Mat &res,
    cv::Rect2f &cropBox)
{
    if (!m_session || !m_memInfo)
    {
        return false;
    }
    cv::imshow("frame", frame);
    static cv::Mat resized;
    cv::resize(frame, resized, cv::Size(m_size, m_size));
    static cv::Mat converted;
    resized.convertTo(converted, CV_32F);
    cv::split(converted, m_channels);

    const auto inputTensor = Ort::Value::CreateTensor(*m_memInfo, m_buffer, 3 * m_size * m_size, m_inputShape.data(), m_inputShape.size());
    const auto output = m_session->Run(Ort::RunOptions{nullptr}, m_inputNames.data(), &inputTensor, m_inputNames.size(), m_outputNames.data(), m_outputNames.size());

    static std::vector<int64_t> strides{{8, 16, 32}};
    auto highestScore = 0.0f;
    auto highestIndex = -1;
    std::vector<cv::Rect> bboxes;
    std::vector<float> scores;
    std::vector<LandmarkDetails> landmarks;
    for (int i = 0; i < strides.size(); ++i)
    {
        const auto stride = strides[i];
        const auto rows = m_size / stride;
        const auto cols = m_size / stride;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                const auto cls = output[i].GetTensorData<float>();
                const auto obj = output[i + 3].GetTensorData<float>();

                const auto idx = r * cols + c;
                const auto score = std::sqrt(cls[idx] + obj[idx]);
                if (score < threshold)
                {
                    continue;
                }
                if (score > highestScore)
                {
                    highestScore = score;
                    ++highestIndex;
                }
                const auto bbox = output[i + 6].GetTensorData<float>();
                const auto cx = (static_cast<float>(c) + bbox[idx * 4]) * static_cast<float>(stride);
                const auto cy = (static_cast<float>(r) + bbox[idx * 4 + 1]) * static_cast<float>(stride);
                const auto cw = std::exp(bbox[idx * 4 + 2]) * static_cast<float>(stride) * 1.5f;
                const auto ch = std::exp(bbox[idx * 4 + 3]) * static_cast<float>(stride) * 1.5f;
                bboxes.emplace_back((cx - cw / 2.0f),
                    (cy - ch / 2.0f),
                    cw,
                    ch);
                scores.emplace_back(score);
                landmarks.emplace_back(c, r, stride, output[i + 9].GetTensorData<float>());
            }
        }
    }
    if (bboxes.empty())
    {
        return false;
    }
    std::vector<int> keepIdx;
    cv::dnn::NMSBoxes(bboxes, scores, threshold, nmsThreshold, keepIdx, eta);
    if (keepIdx.empty())
    {
        return false;
    }

    cv::Rect bbox = bboxes[keepIdx[0]];

    if (bbox.y + bbox.height > resized.rows)
    {
        bbox.height = resized.rows - bbox.y;
    }
    else if (bbox.y < 0)
    {
        bbox.y = 0;
    }
    if (bbox.x + bbox.width > resized.cols)
    {
        bbox.width = resized.cols - bbox.x;
    }
    else if (bbox.x < 0)
    {
        bbox.x = 0;
    }

    cropBox.x = static_cast<float>(bbox.x);
    cropBox.y = static_cast<float>(bbox.y);
    cropBox.width = static_cast<float>(bbox.width);
    cropBox.height = static_cast<float>(bbox.height);
    const auto cropped = resized(cropBox);
    cv::resize(cropped, res, cv::Size(192, 192));
    return true;
}

std::string FaceDetector::getInputInfo() const
{
    return OnnxModelDetails::getInputInfo(*m_session, *m_allocator);
}

std::string FaceDetector::getOutputInfo() const
{
    return OnnxModelDetails::getOutputInfo(*m_session, *m_allocator);
}


