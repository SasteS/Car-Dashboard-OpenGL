#version 330 core
layout(location = 0) in vec4 vertex; // Position and texture coordinates
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model; // For translation only

void main() {
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
