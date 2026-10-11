#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "tools/Macros.h"

SHARED_ONLY(UniformBuffer);

class UniformBuffer
{
public:
    static UniformBufferShared make(int bindingPoint, int bufferSize);

    ~UniformBuffer();

    void uploadData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    void uploadData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 worldMatrix);

    void startUpload();
    void uploadPartialData(int offset, int stride, void* data);
    void endUpload();

private:
    UniformBuffer(int bindingPoint, int bufferSize);

    const int mBufferSize;
    const int mBindingPoint;

    GLuint mBuffer = 0;
};