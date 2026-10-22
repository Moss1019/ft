//
// Created by moss on 11-10-2026.
//

#include "HeadposeEstimator.h"

#include <opencv2/core.hpp>

#include "LandmarkDetector.h"

constexpr std::array<int, 29> kRigidIndices = {
    10, 338, 297, 332, 284, 251, 21, 54, 103, 67, 109,  // forehead / hairline
    151, 9, 8, 168, 6, 197, 195, 5, 4, 1,               // nose bridge + nose
    234, 454, 127, 356,                                  // face sides / temples
    33, 133, 362, 263                                    // eye corners
};

static void normalize(std::vector<glm::vec3> &pts)
{
    glm::vec3 c(0.0f);
    for (const auto &p: pts) c += p;
    c /= static_cast<float>(pts.size());

    float sq = 0.0f;
    for (auto &p: pts)
    {
        p -= c;
        sq += glm::dot(p, p);
    }
    if (const float r = std::sqrt(sq / static_cast<float>(pts.size())); r > 1e-6f)
    {
        for (auto &p: pts) p /= r;
    }
}


std::vector<glm::vec3> HeadposeEstimator::gatherPoints(const LandmarkDetector &landmarks, const cv::Rect2f &cropBox,
    const cv::Size &frameSize, int detectorSize, int landmarkSize)
{
    const float sx = cropBox.width / static_cast<float>(landmarkSize)
                     * static_cast<float>(frameSize.width) / static_cast<float>(detectorSize);
    const float sy = cropBox.height / static_cast<float>(landmarkSize)
                     * static_cast<float>(frameSize.height) / static_cast<float>(detectorSize);
    const float sz = 0.5f * (sx + sy);

    std::vector<glm::vec3> pts;
    pts.reserve(kRigidIndices.size());
    for (const int idx: kRigidIndices)
    {
        const Vec3 v = landmarks.getVec3(idx);
        pts.emplace_back(v.x() * sx, -v.y() * sy, -v.z() * sz);
    }
    return pts;

}

bool HeadposeEstimator::estimate(std::vector<glm::vec3> points, glm::quat &out)
{
    if (points.size() != kRigidIndices.size()) return false;
    normalize(points);

    if (m_reference.empty())
    {
        m_reference = std::move(points);
        out = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        return true;
    }

    cv::Mat H = cv::Mat::zeros(3, 3, CV_64F);
    for (size_t i = 0; i < points.size(); ++i)
    {
        const glm::vec3 &a = m_reference[i];
        const glm::vec3 &b = points[i];
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                H.at<double>(r, c) += static_cast<double>(a[r]) * static_cast<double>(b[c]);
            }
        }
    }

    cv::SVD svd(H);
    const cv::Mat V = svd.vt.t();
    const cv::Mat Ut = svd.u.t();
    cv::Mat D = cv::Mat::eye(3, 3, CV_64F);
    if (cv::determinant(V * Ut) < 0.0) D.at<double>(2, 2) = -1.0;
    const cv::Mat R = V * D * Ut;

    glm::mat3 m;
    for (int r = 0; r < 3; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            m[c][r] = static_cast<float>(R.at<double>(r, c));
        }
    }
    glm::quat q = glm::normalize(glm::quat_cast(m));

    if (m_mirror)
    {
        q = glm::quat(q.w, q.x, -q.y, -q.z);
    }

    m_smoothed = glm::normalize(glm::slerp(m_smoothed, q, m_alpha));
    out = m_smoothed;
    return true;
}
