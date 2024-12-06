#version 330 core
out vec4 FragColor;

// Uniform variable to dynamically set the color
uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0); // Use the uniform color with full opacity
}
