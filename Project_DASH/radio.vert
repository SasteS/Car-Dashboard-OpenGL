//#version 330 core
//layout(location = 0) in vec3 aPos;
//
//uniform mat4 projection;
//
//void main() {
//    gl_Position = projection * vec4(aPos, 1.0);
//}
//

#version 330 core
layout(location = 0) in vec3 aPos;   // Position
layout(location = 1) in vec3 aColor; // Color

out vec3 vertexColor; // Pass color to fragment shader

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPos, 1.0);
    vertexColor = aColor;
}
