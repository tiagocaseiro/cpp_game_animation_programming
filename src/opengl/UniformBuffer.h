#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <glad/glad.h>

#include "tools/Macros.h"

OPTIONAL_ONLY(UniformBuffer);

template <typename... Types>
struct uniform_buffer_size
{
private:
    template <size_t Size, typename A, typename... Types>
    struct uniform_buffer_size_internal : uniform_buffer_size_internal<Size + sizeof(A), Types...>
    {
    };

    template <size_t Size, typename A>
    struct uniform_buffer_size_internal<Size, A>
    {
        static constexpr size_t value = Size + sizeof(A);
    };

public:
    static constexpr size_t value = uniform_buffer_size_internal<0, Types...>::value;
};

template <typename... Types>
constexpr size_t uniform_buffer_size_v = uniform_buffer_size<Types...>::value;

class UniformBuffer
{
public:
    static UniformBufferOptional make(int bindingPoint, int bufferSize);

    template <typename... Types>
    static UniformBufferOptional make(int bindingPoint)
    {
        static constexpr size_t bufferSize = uniform_buffer_size_v<Types...>;
        return make(bindingPoint, bufferSize);
    }

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