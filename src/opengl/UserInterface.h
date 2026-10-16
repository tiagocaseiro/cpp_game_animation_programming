#pragma once

#include "RenderData.h"

class UserInterface
{
public:
    UserInterface(const RenderData& renderData);
    ~UserInterface();
    void createFrame(RenderData& renderData);
    void render();

private:
    float framesPerSecond = 0.0f;
    float averagingAlpha  = 0.96f;
};