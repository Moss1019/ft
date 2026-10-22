#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

#include "detection/FaceDetector.h"
#include "detection/FaceTracker.h"
#include "detection/LandmarkDetector.h"
#include "game/Avatar.h"
#include "game/Engine.h"
#include "glb/Vertex.h"
#include "glb/GlbReader.h"
#include "glb/Skeleton.h"
#include "timer/Timer.h"

void faceDetector()
{
    constexpr std::array<int, 29> kRigidIndices = {
        10, 338, 297, 332, 284, 251, 21, 54, 103, 67, 109,  // forehead / hairline
        151, 9, 8, 168, 6, 197, 195, 5, 4, 1,               // nose bridge + nose
        234, 454, 127, 356,                                  // face sides / temples
        33, 133, 362, 263                                    // eye corners
    };

    cv::VideoCapture cap("/dev/video2", cv::CAP_V4L);

    Timer gameLoopTimer;
    gameLoopTimer.start();

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "face_tracker");
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    FaceDetector fd(env, sessionOptions, &memoryInfo, "face_detector.onnx");
    LandmarkDetector ld(env, sessionOptions, &memoryInfo, "face_landmark_detector.onnx");

    cv::Mat frame;
    cv::Mat cropped;
    cv::Rect2f cropBox;

    std::set<int> rigidIndices;
    for (const auto &i : kRigidIndices)
    {
        rigidIndices.insert(i);
    }

    while (cap.read(frame))
    {
        if (fd.detect(frame, 1.25f, 0.8f, 0.95f, cropped, cropBox))
        {
            if (ld.detect(cropped))
            {
                auto xScale = static_cast<float>(frame.cols) / 320.0f;
                auto yScale = static_cast<float>(frame.rows) / 320.0f;

                for (int i = 0; i < 468; ++i)
                {

                    const auto vec = ld.getVec3(i);
                    const float frameX = (cropBox.x + vec.x() * cropBox.width) * xScale;
                    const float frameY = (cropBox.y + vec.y() * cropBox.height) * yScale;

                    const auto color = rigidIndices.contains(i) ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0);
                    cv::circle(frame, cv::Point(cvRound(frameX), cvRound(frameY)), 1, color, -1);
                }

                // for (int ind = 0; ind < kRigidIndices.size(); ++ind)
                // {
                //     const auto i = kRigidIndices[ind];
                //     const auto vec = ld.getVec3(i);
                //     const float frameX = (cropBox.x + vec.x() * cropBox.width) * xScale;
                //     const float frameY = (cropBox.y + vec.y() * cropBox.height) * yScale;
                //
                //     cv::circle(frame, cv::Point(cvRound(frameX), cvRound(frameY)), 1, cv::Scalar(0, 255, 0), -1);
                //     // cv::putText(frame, cv::format("%d", i), cv::Point(static_cast<int>(frameX), static_cast<int>(frameY)), cv::FONT_HERSHEY_COMPLEX, 0.125, cv::Scalar(255, 255, 255));
                // }
            }
        }
        cv::imshow("frame", frame);

        gameLoopTimer.start();
        if (cv::waitKey(13) == 27) break;
    }

    cv::destroyAllWindows();
}

static void game()
{
    std::string boxPath = "../box.glb";
    std::string box2Path = "../box2.glb";
    std::string planePath = "../plane.glb";
    std::string avatarPath = "../avatar.glb";

    Engine eng(800, 600);
    std::vector modelPaths{{avatarPath}};

    std::map<std::string, GameObjectInfo> modelInfo;
    std::map<std::string, std::unique_ptr<Skeleton>> modelSkeletons;

    for (const auto& modelPath : modelPaths)
    {
        GlbReader reader(modelPath);

        std::vector<Vertex> vertices;
        reader.vertexData(vertices);

        std::vector<GLuint> indices;
        reader.indices(indices);

        NodeInfo nodeInfo;
        reader.nodeHierarchy(nodeInfo);

        JointInfo jointInfo;
        reader.skinJoints(jointInfo);

        modelInfo[modelPath] = eng.addGameObjectModel(vertices, indices);
        modelSkeletons[modelPath] = std::make_unique<Skeleton>(Skeleton(nodeInfo, jointInfo));
        reader.skeleton(*modelSkeletons[modelPath]);
    }

    cv::VideoCapture cap("/dev/video2", cv::CAP_V4L);

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "face_tracker");
    Ort::SessionOptions sessionOptions;
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
    sessionOptions.AddConfigEntry("session.intra_op.allow_spinning", "0");
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    FaceDetector fd(env, sessionOptions, &memoryInfo, "face_detector.onnx");
    LandmarkDetector ld(env, sessionOptions, &memoryInfo, "face_landmark_detector.onnx");
    HeadposeEstimator estimator;
    FaceTracker ft(&cap, &fd, &ld, &estimator);
    ft.start();

    Avatar box(modelInfo[avatarPath]);
    box.setPosition({0.0f, 0.0f, 0.0f});
    box.setSkeleton(std::move(modelSkeletons[avatarPath]));
    box.setFaceTracker(&ft);
    eng.addGameObject(&box);

    if (eng.inError())
    {
        std::cout << "Engine error: " << eng.error() << "\n";
    }
    else
    {
        if (!eng.run())
        {
            std::cout << "Engine error: " << eng.error() << "\n";
        }
    }

    ft.stop();
    cv::destroyAllWindows();
}

static void printMat4(const glm::mat4 &mat)
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            std::cout << mat[col][row] << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    // faceDetector();
    game();
    return 0;
}
