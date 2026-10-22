
#include "Avatar.h"

#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>

#include "Smoothing.h"
#include "../detection/FaceDetector.h"
#include "../detection/FaceTracker.h"
#include "../detection/HeadposeEstimator.h"
#include "../detection/LandmarkDetector.h"
#include "../gl/Program.h"
#include "../glb/Skeleton.h"

Avatar::Avatar(const GameObjectInfo &info)
    :GameObject(info)
{

}

Avatar::~Avatar() = default;

void Avatar::setSkeleton(std::unique_ptr<Skeleton> skeleton)
{
    m_skeleton = std::move(skeleton);
}

void Avatar::setJointRotation(int32_t nodeIdx, const glm::quat &q) const
{
    if (m_skeleton) m_skeleton->setRotation(nodeIdx, q);
}

void Avatar::setJointTranslation(int32_t nodeIdx, const glm::vec3 &t) const
{
    if (m_skeleton) m_skeleton->setTranslation(nodeIdx, t);
}

void Avatar::setJointScale(int32_t nodeIdx, const glm::vec3 &s) const
{
    if (m_skeleton) m_skeleton->setScale(nodeIdx, s);
}

void Avatar::onUpdate(double dt)
{
    const auto fdt = static_cast<float>(dt);

    if (m_ft && m_ft->isRunning())
    {
        m_headTarget = m_ft->headRotation();
    }
    m_headRotation = smoothTowards(m_headRotation, m_headTarget, m_headSpeed, fdt);

    if (m_skeleton)
    {
        setJointRotation(m_headJoint, m_headRotation);
    }

    // static cv::Mat frame;
    // static cv::Mat cropped;
    // static cv::Rect2f cropBox;
    //
    // const auto fdt = static_cast<float>(dt);
    //
    // if (m_videoCapture && m_faceDetector && m_landmarkDetector)
    // {
    //     m_videoCapture->read(frame);
    //     if (m_faceDetector->detect(frame, 1.25f, 0.8f, 0.95f, cropped, cropBox))
    //     {
    //         if (m_landmarkDetector->detect(cropped))
    //         {
    //             // m_landmarkDetector gives the vec3 positions with getVec3(index) where 0 <= index < 468
    //         }
    //     }
    // }
    //
    // if (m_skeleton)
    // {
    //     // Set joint index 0 to match the rotation of the face detected
    // }
}

void Avatar::onPreRender()
{
    if (m_skeleton)
    {
        m_skeleton->computeJointMatrices(m_jointMatrices);
    }
}
