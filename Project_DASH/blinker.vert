#version 330 core

layout(location = 0) in vec2 aPos;    // Vertex positions (x, y)
out vec3 color;

uniform mat4 model;
uniform mat4 projection;
uniform vec3 inputColor;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);

    color = inputColor;
}
