#pragma once
#include <thread>
#include <opencv2/videoio.hpp>

#include <glm/gtc/quaternion.hpp>

class MotionGate;
class HeadposeEstimator;
class LandmarkDetector;
class FaceDetector;

class FaceTracker
{
public:
    explicit FaceTracker(cv::VideoCapture *cap, FaceDetector *fd, LandmarkDetector *ld, HeadposeEstimator *hp);

    ~FaceTracker();

    [[nodiscard]]
    bool isRunning() const { return m_isRunning; }

    [[nodiscard]]
    glm::quat headRotation();

    void start();

    void stop();

    void publish(const glm::quat &q);

    void track();

private:
    std::thread *m_worker = nullptr;

    cv::VideoCapture *m_cap = nullptr;

    FaceDetector *m_fd = nullptr;

    LandmarkDetector *m_ld = nullptr;

    HeadposeEstimator *m_hp = nullptr;

    MotionGate *m_mg = nullptr;

    glm::quat m_headRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    bool m_isRunning = false;

    std::mutex m_mutex;

    void doWork();
};
