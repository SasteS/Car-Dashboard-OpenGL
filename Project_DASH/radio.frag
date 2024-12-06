//#version 330 core
//out vec4 FragColor;
//
//uniform vec3 color;
//
//void main() {
//    FragColor = vec4(color, 1.0);
//}
//

#version 330 core
in vec3 vertexColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0); // Use interpolated color
}
