#pragma once

#include <memory>

#include "GameObject.h"
#include "../detection/HeadposeEstimator.h"

class FaceTracker;
class LandmarkDetector;
class FaceDetector;

namespace cv
{
    class VideoCapture;
}

class Avatar : public GameObject
{
public:
    explicit Avatar(const GameObjectInfo &info);

    ~Avatar() override;

    [[nodiscard]]
    const Skeleton *skeleton() const { return m_skeleton.get(); }

    void setSkeleton(std::unique_ptr<Skeleton> skeleton);

    void setFaceTracker(FaceTracker *value) { m_ft = value; }

    [[nodiscard]]
    const glm::vec3 &velocity() const { return m_velocity; }

    void setVelocity(const glm::vec3 &v) { m_velocity = v; }

    void setHeadSpeed(float s) { m_headSpeed = s; }

    void setJointRotation(int32_t nodeIdx, const glm::quat &q) const;

    void setJointTranslation(int32_t nodeIdx, const glm::vec3 &t) const;

    void setJointScale(int32_t nodeIdx, const glm::vec3 &s) const;

protected:
    void onUpdate(double dt) override;

    void onPreRender() override;

private:
    std::unique_ptr<Skeleton> m_skeleton;

    glm::vec3 m_velocity{0.0f};

    glm::quat m_headTarget{1.0f, 0.0f, 0.0f, 0.0f};

    glm::quat m_headRotation{1.0f, 0.0f, 0.0f, 0.0f};

    float m_headSpeed = 14.0f;

    FaceTracker *m_ft = nullptr;

    int32_t m_headJoint = 0;

};
