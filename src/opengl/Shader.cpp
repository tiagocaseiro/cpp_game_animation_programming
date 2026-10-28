#include "Shader.h"

#include <fstream>

static GLuint readShader(const std::string shaderFilename, GLuint shaderType)
{
    std::string shaderAsText;
    std::ifstream inFile(shaderFilename);

    if(inFile.is_open())
    {
        inFile.seekg(0, std::ios::end);
        shaderAsText.reserve(inFile.tellg());
        inFile.seekg(0, std::ios::beg);
        shaderAsText.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
    }
    else
    {
        return 0;
    }

    if(inFile.bad() || inFile.fail())
    {
        inFile.close();
        return 0;
    }

    inFile.close();

    const char* shaderSource = shaderAsText.c_str();

    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);
    glCompileShader(shader);

    GLint isShaderCompiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
    if(!isShaderCompiled)
    {
        return 0;
    }

    return shader;
}

ShaderShared Shader::make(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    GLuint vertexShader = readShader(vertexShaderFilename, GL_VERTEX_SHADER);
    if(!vertexShader)
    {
        return nullptr;
    }
    GLuint fragmentShader = readShader(fragmentShaderFilename, GL_FRAGMENT_SHADER);
    if(!fragmentShader)
    {
        return nullptr;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint isProgramLinked;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isProgramLinked);

    if(!isProgramLinked)
    {
        return nullptr;
    }

    ShaderShared shader = ShaderShared(new Shader(shaderProgram));

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

Shader::Shader(GLuint shaderProgram) : mShaderProgram(shaderProgram) {}

void Shader::use() { glUseProgram(mShaderProgram); }

Shader::~Shader() { glDeleteProgram(mShaderProgram); }

bool Shader::initUniformLocation(std::string const& uniformName)
{
    if(mShaderProgram > 0)
    {
        mUniformLocation = glGetUniformLocation(mShaderProgram, uniformName.c_str());
        return mUniformLocation.has_value();
    }

    return false;
}

void Shader::setUniformValue(int const value)
{
    if(mShaderProgram > 0 && mUniformLocation.has_value())
    {
        glUniform1i(*mUniformLocation, value);
    }
}