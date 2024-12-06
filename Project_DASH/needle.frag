#version 330 core
out vec4 FragColor;

uniform vec3 needleColor;

void main() {
    FragColor = vec4(needleColor, 1.0f);
}
