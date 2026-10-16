#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix(RenderData& renderData)
{
    const float azimRad = glm::radians(renderData.rdViewAzimuth);
    const float elevRad = glm::radians(renderData.rdViewElevation);

    const float sinAzim = glm::sin(azimRad);
    const float cosAzim = glm::cos(azimRad);
    const float sinElev = glm::sin(elevRad);
    const float cosElev = glm::cos(elevRad);

    mViewDirection = glm::normalize(glm::vec3(sinAzim * cosElev, sinElev, -cosAzim * cosElev));

    static const glm::vec3 mWorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

    mRightDirection = glm::normalize(glm::cross(mViewDirection, mWorldUpVector));
    mUpDirection    = glm::normalize(glm::cross(mRightDirection, mViewDirection));

    renderData.rdCameraWorldPosition += renderData.rdTickDiff * renderData.rdMoveForward * mViewDirection +
                                        renderData.rdTickDiff * renderData.rdMoveRight * mRightDirection +
                                        renderData.rdTickDiff * renderData.rdMoveUp * mUpDirection;

    return glm::lookAt(renderData.rdCameraWorldPosition, renderData.rdCameraWorldPosition + mViewDirection,
                       mUpDirection);
}
