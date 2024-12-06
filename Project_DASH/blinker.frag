#version 330 core

in vec3 color;
out vec4 FragColor;

uniform float alpha;

void main() {
    FragColor = vec4(color, alpha);
}
