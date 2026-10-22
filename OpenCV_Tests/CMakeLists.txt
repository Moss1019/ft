cmake_minimum_required(VERSION 4.4)
project(OpenCV_Tests)

set(CMAKE_CXX_STANDARD 20)
set(OpenCV_DIR $ENV{HOME}/libraries/opencv-install/lib/cmake/opencv4)
set(ONNX_FILES
        ${CMAKE_SOURCE_DIR}/face_landmark_detector.onnx
        ${CMAKE_SOURCE_DIR}/face_detector.onnx
)
set(SHADER_FILES
        ${CMAKE_SOURCE_DIR}/vertex.glsl
        ${CMAKE_SOURCE_DIR}/fragment.glsl
)

foreach(ONNX ${ONNX_FILES})
    get_filename_component(ONNX_NAME ${ONNX} NAME)
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/${ONNX_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy ${ONNX} ${CMAKE_BINARY_DIR}/${ONNX_NAME}
            DEPENDS ${ONNX}
            COMMENT "Copying ONNX ${ONNX_NAME}"
    )
    list(APPEND ONNX_OUTPUTS ${CMAKE_BINARY_DIR}/${ONNX_NAME})
endforeach()
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/${SHADER_NAME}
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_BINARY_DIR}/${SHADER_NAME}
            DEPENDS ${SHADER}
            COMMENT "Copying shader ${SHADER_NAME}"
    )
    list(APPEND SHADER_OUTPUTS ${CMAKE_BINARY_DIR}/${SHADER_NAME})
endforeach()

add_library(glad STATIC lib/glad/glad.c)
add_library(tinyglb STATIC lib/tinygltf/tiny_gltf_v3.c)

find_package(OpenCV REQUIRED)
find_library(ONNXRUNTIME_LIB onnxruntime PATHS lib/onnx/lib)

target_include_directories(glad PUBLIC lib)
target_include_directories(tinyglb PUBLIC include/tinyglft)
target_compile_definitions(tinyglb PUBLIC TINYGLTF3_ENABLE_FS)

add_executable(OpenCV_Tests main.cpp
        timer/Timer.cpp
        timer/Timer.h
        detection/FaceDetector.cpp
        detection/FaceDetector.h
        details/OnnxModelDetails.cpp
        details/OnnxModelDetails.h
        detection/LandmarkDetector.cpp
        detection/LandmarkDetector.h
        file/FileHandler.cpp
        file/FileHandler.h
        gl/Window.cpp
        gl/Window.h
        gl/Shader.cpp
        gl/Shader.h
        file/FileHandler.h
        gl/Program.cpp
        gl/Program.h
        glb/GlbReader.cpp
        glb/GlbReader.h
        glb/Skeleton.cpp
        glb/Skeleton.h
        glb/Vertex.cpp
        glb/Vertex.h
        gl/IndexBuffer.cpp
        gl/IndexBuffer.h
        gl/VertexBuffer.cpp
        gl/VertexBuffer.h
        gl/Light.cpp
        gl/Light.h
        game/Engine.cpp
        game/Engine.h
        game/GameObject.cpp
        game/GameObject.h
        game/GameObjectInfo.h
        game/Avatar.cpp
        game/Avatar.h
        game/Transform.cpp
        game/Transform.h
        detection/FaceTracker.cpp
        detection/FaceTracker.h
        detection/HeadposeEstimator.cpp
        detection/HeadposeEstimator.h
        detection/MotionGate.cpp
        detection/MotionGate.h
        game/Smoothing.cpp
        game/Smoothing.h)

target_include_directories(OpenCV_Tests PRIVATE lib/onnx/include)

target_link_libraries(OpenCV_Tests PRIVATE ${OpenCV_LIBS} ${ONNXRUNTIME_LIB} GL glad glfw tinyglb)

add_custom_target(copy_models ALL DEPENDS ${ONNX_OUTPUTS})
add_custom_target(copy_shaders ALL DEPENDS ${SHADER_OUTPUTS})

add_dependencies(OpenCV_Tests copy_models)
add_dependencies(OpenCV_Tests copy_shaders)

# Enable testing
enable_testing()

# Add tests directory
add_subdirectory(tests)
