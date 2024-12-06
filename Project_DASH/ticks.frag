#version 330 core
out vec4 FragColor;

uniform vec3 tickColor; // Dynamic color for tick marks
uniform float opacity;

void main() {
    FragColor = vec4(tickColor, opacity);
}
