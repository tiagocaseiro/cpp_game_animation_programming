#pragma once

#include <glm/gtc/type_ptr.hpp>

#include "opengl/RenderData.h"

struct ModelSettings
{
    glm::vec3 msWorldPosition    = glm::vec3();
    glm::vec3 msWorldRotation    = glm::vec3();
    bool msDrawModel             = true;
    bool msPlayAnimation         = true;
    int msAnimClip               = 0;
    float msAnimSpeed            = 1.0f;
    float msAnimTimePosition     = 0.0f;
    float msAnimEndTime          = 0.0f;
    BlendMode msBlendMode        = BlendMode::FadeInOut;
    float msAnimBlendFactor      = 1.0f;
    int msCrossBlendDestAnimClip = 0;
    float msCrossBlendFactor     = 0.0f;
    int msSkelSplitNode          = 0;
    IKMode msIKMode              = IKMode::OFF;
    int msIKNumIterations        = 10;
    glm::vec3 msIKTargetPosition = glm::vec3(0.0f, 3.0f, 1.0f);
    int msIKEffectorNode         = 0;
    int msIKRootNode             = 0;
    glm::vec3 msIkTargetWorldPos = glm::vec3(0.0f, 0.0f, 1.0f);

    std::vector<char const*> msAnimationClipNames;
    std::vector<char const*> msSkelSplitNodeNames;
    // glm::vec3 msIKTargetPosition = glm::vec3(-1.972f, 1.631f, 0.726f);
    // AnimationReplayDirection msReplayDirect = AnimationReplayDirection::Forward;
};