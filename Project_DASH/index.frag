#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D text; // Texture containing the glyph
uniform vec3 textColor;

void main() {    
    float alpha = texture(text, TexCoords).r;
    
    FragColor = vec4(textColor, alpha);
}
