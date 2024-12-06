#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D text; // Texture containing the glyph
uniform vec3 textColor; // Color for the text

void main() {
    // Sample the alpha channel of the glyph texture
    float alpha = texture(text, TexCoords).r;
    
    // Set the fragment color
    FragColor = vec4(textColor, alpha);
}
