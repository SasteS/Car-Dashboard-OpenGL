#version 330 core

layout (location = 0) in vec4 vertex; // (x, y, z, texCoords)

uniform mat4 projection; // Projection matrix

out vec2 TexCoords;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); // Project vertex position
    TexCoords = vertex.zw; // Pass texture coordinates to the fragment shader
}
