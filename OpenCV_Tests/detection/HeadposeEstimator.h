#pragma once
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <opencv2/core/types.hpp>

class LandmarkDetector;

class HeadposeEstimator
{
public:
    static std::vector<glm::vec3> gatherPoints(const LandmarkDetector &landmarks,
                                               const cv::Rect2f &cropBox,
                                               const cv::Size &frameSize,
                                               int detectorSize,
                                               int landmarkSize = 192);

    bool estimate(std::vector<glm::vec3> points, glm::quat &out);

    void recalibrate() { m_reference.clear(); }

    void setSmoothing(float alpha) { m_alpha = alpha; }

    void setMirror(bool mirror) { m_mirror = mirror; }
private:
    std::vector<glm::vec3> m_reference;

    glm::quat m_smoothed{1.0f, 0.0f, 0.0f, 0.0f};

    float m_alpha = 0.5f;

    bool m_mirror = true;

};
