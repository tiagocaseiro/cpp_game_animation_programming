#pragma once

#include <glad/glad.h>

#include "RenderData.h"

class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();
    void uploadData(const Mesh& vertexData);
    void bind();
    void unbind();
    void draw(GLuint mode, const int start, const int num);

private:
    GLuint mVAO       = 0;
    GLuint mVertexVBO = 0;
};