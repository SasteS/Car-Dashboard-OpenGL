#version 330 core

layout(location = 0) in vec2 aPos; // Input vertex position
uniform mat4 projection;          // Projection matrix

out vec2 fragPos; // Pass to fragment shader

void main()
{
    fragPos = aPos; // Pass normalized position to fragment shader
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
