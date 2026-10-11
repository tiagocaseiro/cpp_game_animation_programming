#include "UniformBuffer.h"

#include <glm/gtc/type_ptr.hpp>

UniformBuffer::UniformBuffer(int bindingPoint, int bufferSize) : mBufferSize(bufferSize), mBindingPoint(bindingPoint)
{
    glGenBuffers(1, &mBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
    glBufferData(GL_UNIFORM_BUFFER, mBufferSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer() { glDeleteBuffers(1, &mBuffer); }

void UniformBuffer::uploadData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mBuffer, 0, mBufferSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::uploadData(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 worldMatrix)
{
    glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(worldMatrix));
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mBuffer, 0, mBufferSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::startUpload() { glBindBuffer(GL_UNIFORM_BUFFER, mBuffer); }

void UniformBuffer::uploadPartialData(int offset, int size, void* data)
{
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UniformBuffer::endUpload()
{
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mBuffer, 0, mBufferSize);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBufferShared UniformBuffer::make(int bindingPoint, int bufferSize)
{
    return UniformBufferShared(new UniformBuffer(bindingPoint, bufferSize));
}
