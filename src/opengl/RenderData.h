#pragma once

#include <string>

#include <glm/glm.hpp>

#include "model/GltfNode.h"

struct GLFWwindow;

enum class BlendMode
{
    FadeInOut = 0,
    CrossFade,
    Additive
};

enum class IKMode
{
    OFF = 0,
    CCD,
    FABRIK
};

struct VertexData
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 uv;
};

struct Mesh
{
    std::vector<VertexData> vertices;
};

struct RenderData
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
    std::vector<char const*> rdNodeNames;

    bool rdPlayAnimation                 = false;
    int rdAnimClip                       = 5;
    float rdAnimDurationMs               = 0.0f;
    int rdAnimClipSize                   = 0;
    float rdAnimSpeed                    = 1.0f;
    float rdAnimTimePosition             = 0.0f;
    float rdAnimEndTime                  = 0.0f;
    float rdAnimBlendFactor              = 0.0f;
    BlendMode rdBlendMode                = BlendMode::FadeInOut;
    int rdCrossBlendDestAnimClip         = 7;
    float rdCrossBlendDestAnimDurationMs = 0.0f;
    std::string rdCrossBlendDestAnimName = "None";
    float rdAnimCrossBlendFactor         = 1.0f;

    int rdSkelSplitNode     = 29;
    int rdDebugSelectedNode = 0;

    GltfNodeShared rdRootNode;

    glm::vec3 rdCameraWorldPosition = glm::vec3(1.5f, 4.0f, 4.5f);

    bool rdShowMesh          = true;
    bool rdShowNodeDebugMenu = true;

    IKMode rdIkMode         = IKMode::OFF;
    int rdIkNumIterations   = 10;
    glm::vec3 rdIkTargetPos = glm::vec3(-1.972f, 1.631f, 0.726f);

    int rdIkEffectorNode = 19;
    int rdIkRootNode     = 25;
};