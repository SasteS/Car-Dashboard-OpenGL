#version 330 core

layout(location = 0) in vec2 aPos;    // Vertex positions (x, y)
out vec3 color;                       // Pass color to fragment shader

uniform mat4 model;                   // Model matrix for positioning
uniform mat4 projection;              // Projection matrix (for screen space)
uniform vec3 inputColor;              // Color for the blinker (passed from CPU)

// Main function
void main() {
    // Apply transformation (projection and model) to the vertex position
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);

    // Pass the color to the fragment shader
    color = inputColor;
}
