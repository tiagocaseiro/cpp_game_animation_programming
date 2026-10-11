
layout(std140, binding = 0) uniform SharedData
{
    mat4 view;
    mat4 projection;
    mat4 world;
    int selected;
};

out vec4 FragColor;

void main() { FragColor = (selected == 0) ? vec4(1.0, 0.0, 0.0, 1.0) : vec4(0.0, 1.0, 0.0, 1.0); }