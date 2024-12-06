#version 330 core

in vec3 color;       // Color passed from the vertex shader
out vec4 FragColor;  // Final fragment color

uniform float alpha; // Alpha value (opacity)

void main() {
    FragColor = vec4(color, alpha); // Use the alpha uniform for transparency
}
