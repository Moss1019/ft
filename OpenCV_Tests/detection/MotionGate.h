#pragma once
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

class MotionGate
{
public:
    explicit MotionGate(cv::Size smallSize = cv::Size(80, 60),
                        double pixelThreshold = 20.0,
                        double changeFactor = 0.01,
                        int holdFrames = 0,
                        int maxSkipFrames = 30)
        : m_smallSize(smallSize),
          m_pixelThreshold(pixelThreshold),
          m_changeFactor(changeFactor),
          m_holdFrames(holdFrames),
          m_maxSkipFrames(maxSkipFrames)
    {}

    [[nodiscard]]
    bool shouldRun(const cv::Mat &frame);

    void reset() { m_keyframe.release(); }

private:
    cv::Size m_smallSize;

    double m_pixelThreshold;

    double m_changeFactor;

    int m_holdFrames;

    int m_maxSkipFrames;

    cv::Mat m_small, m_gray, m_diff, m_keyframe;

    int m_hold = 0;

    int m_skipped = 0;

    bool trigger();
};
