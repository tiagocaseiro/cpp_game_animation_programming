#pragma once

#include "opengl/RenderData.h"

class Camera
{
public:
    glm::mat4 getViewMatrix(RenderData& renderData);

private:
    glm::vec3 mViewDirection  = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mRightDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mUpDirection    = glm::vec3(0.0f, 0.0f, 0.0f);
};