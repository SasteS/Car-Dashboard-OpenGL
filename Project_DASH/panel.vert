#version 330 core

layout(location = 0) in vec3 aPosition;  // Vertex position

uniform mat4 projection; // Projection matrix

void main() {
    gl_Position = projection * vec4(aPosition, 1.0);
}
