//
// Created by moss on 12-10-2026.
//

#include "MotionGate.h"

#include <opencv2/imgproc.hpp>

#include <iostream>

bool MotionGate::shouldRun(const cv::Mat &frame)
{
    cv::resize(frame, m_small, m_smallSize, 0, 0, cv::INTER_AREA);
    if (m_small.channels() == 3)
    {
        cv::cvtColor(m_small, m_gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        m_small.copyTo(m_gray);
    }
    cv::GaussianBlur(m_gray, m_gray, {5, 5}, 0);
    if (m_keyframe.empty())
    {
        return trigger();
    }

    cv::absdiff(m_keyframe, m_gray, m_diff);
    cv::threshold(m_diff, m_diff, m_pixelThreshold, 255, cv::THRESH_BINARY);
    const auto fraction  = static_cast<double>(cv::countNonZero(m_diff)) / static_cast<double>(m_diff.total());

    if (fraction > m_changeFactor)
    {
        return trigger();
    }
    if (m_hold > 0)
    {
        --m_hold;
        m_skipped = 0;
        return true;
    }
    return false;
}

bool MotionGate::trigger()
{
    m_gray.copyTo(m_keyframe);
    m_hold = m_holdFrames;
    m_skipped = 0;
    return true;
}
