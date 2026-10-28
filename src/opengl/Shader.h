#pragma once

#include <optional>
#include <string>

#include <glad/glad.h>

#include "tools/Macros.h"

SHARED_ONLY(Shader);

class Shader
{
public:
    static ShaderShared make(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
    void use();
    ~Shader();

    bool initUniformLocation(std::string const& uniformName);
    void setUniformValue(int const value);

private:
    Shader(GLuint mShaderProgram);

    const GLuint mShaderProgram = 0;
    std::optional<int> mUniformLocation;
};
