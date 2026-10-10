#pragma once

#include <string>

#include <glm/glm.hpp>

struct GLFWwindow;

struct OGLVertexData
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv;
};

struct OGLMesh
{
    std::vector<OGLVertexData> vertices;
};

struct OGLRenderData
{
    GLFWwindow* rdWindow             = nullptr;
    unsigned int rdWidth             = 0;
    unsigned int rdHeight            = 0;
    unsigned int rdTriangleCount     = 0;
    unsigned int rdGltfTriangleCount = 0;
    float rdFrameTime                = 0.0f;
    float rdUIGenerateTime           = 0.0f;
    float rdViewAzimuth              = 320.0f;
    float rdViewElevation            = -15.0f;
    int rdMoveForward                = 0;
    int rdMoveRight                  = 0;
    int rdMoveUp                     = 0;
    float rdTickDiff                 = 0.0;
    int rdAnimationClipSize          = 0;
    int rdNodeCount                  = 0;

    std::vector<char const*> rdAnimationClipNames;

    bool rdPlayAnimation                 = true;
    std::string rdClipName               = "None";
    int rdAnimClip                       = 5;
    float rdAnimDurationMs               = 0.0f;
    int rdAnimClipSize                   = 0;
    float rdAnimSpeed                    = 1.0f;
    float rdAnimTimePosition             = 0.0f;
    float rdAnimEndTime                  = 0.0f;
    float rdAnimBlendFactor              = 1.0f;
    bool rdCrossBlending                 = false;
    int rdCrossBlendDestAnimClip         = 7;
    float rdCrossBlendDestAnimDurationMs = 0.0f;
    std::string rdCrossBlendDestAnimName = "None";
    float rdAnimCrossBlendFactor         = 1.0f;

    bool rdAdditiveBlending         = false;
    int rdSkelSplitNode             = 29;
    std::string rdSkelSplitNodeName = "None";

    glm::vec3 rdCameraWorldPosition = glm::vec3(1.5f, 4.0f, 4.5f);
};