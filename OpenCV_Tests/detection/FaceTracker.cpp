//
// Created by moss on 11-10-2026.
//

#include "FaceTracker.h"

#include "FaceDetector.h"
#include "HeadposeEstimator.h"
#include "LandmarkDetector.h"
#include "MotionGate.h"

FaceTracker::FaceTracker(cv::VideoCapture *cap, FaceDetector *fd, LandmarkDetector *ld, HeadposeEstimator *hp)
    :m_cap(cap), m_fd(fd), m_ld(ld), m_hp(hp)
{
    m_mg = new MotionGate({80, 60}, 10, 0.005, 0, 15);
}

FaceTracker::~FaceTracker()
{
    stop();
    m_worker->join();
    delete m_mg;
    delete m_worker;
}

glm::quat FaceTracker::headRotation()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_headRotation;
}

void FaceTracker::start()
{
    m_isRunning = true;
    m_worker = new std::thread(&FaceTracker::doWork, this);
}

void FaceTracker::stop()
{
    m_isRunning = false;
}

void FaceTracker::publish(const glm::quat &q)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_headRotation = q;
}

void FaceTracker::track()
{
    static cv::Mat frame;
    static cv::Mat cropped;
    static cv::Rect2f cropBox;

    if (m_cap && m_fd && m_ld)
    {
        if (m_cap->read(frame) && !frame.empty()
            && m_mg->shouldRun(frame)
            && m_fd->detect(frame, 1.25f, 0.8f, 0.95f, cropped, cropBox)
            && m_ld->detect(cropped))
        {
            auto pts = HeadposeEstimator::gatherPoints(*m_ld, cropBox,
                                                       frame.size(), m_fd->size());
            if (glm::quat head; m_hp->estimate(std::move(pts), head))
            {
                publish(head);
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void FaceTracker::doWork()
{
    while (m_isRunning)
    {
        track();
    }
}
