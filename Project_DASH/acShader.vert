#version 330 core

layout(location = 0) in vec2 aPos;
uniform mat4 projection;

out vec2 fragPos;

void main()
{
    fragPos = aPos;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
