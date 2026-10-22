// vertex shader

#version 440 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aWeights;
layout(location = 3) in uvec4 aJoints;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform mat4[2] uBindMats;

uniform mat3 uNormalMat;

out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    mat4 skinMatrix = uBindMats[aJoints.x] * aWeights.x +
           uBindMats[aJoints.y] * aWeights.y +
           uBindMats[aJoints.z] * aWeights.z +
           uBindMats[aJoints.w] * aWeights.w;

    vec4 skinnedPos = skinMatrix * vec4(aPosition, 1.0);
    vec3 skinnedNormal = mat3(skinMatrix) * aNormal;

    vec4 worldPos = uModel * skinnedPos;

    vFragPos = worldPos.xyz;
    vNormal = normalize(uNormalMat * skinnedNormal);

    gl_Position = uProjection * uView * worldPos;
}
