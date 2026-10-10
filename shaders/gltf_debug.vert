#version 460 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 projection;
    mat4 world;
};

void main() { gl_Position = projection * view * world * vec4(aPos.x, aPos.y, aPos.z, 1.0); }
