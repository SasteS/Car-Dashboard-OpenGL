#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColorStart;
uniform vec3 textColorEnd;

uniform bool isGearText;          // Flag to indicate if the text is "gear"
uniform float time;               // Time variable for periodic color change

void main() {    
    // Sample the texture
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    
    // Determine the gradient factor based on texture coordinates
    float gradientFactor = (TexCoords.x + TexCoords.y) * 0.5;    
    // Default gradient color
    vec3 gradientColor = mix(textColorStart, textColorEnd, gradientFactor);

    // Override color for "gear" text
    if (isGearText) {
        // Periodic color oscillation based on time
        float oscillation = sin(time) * 0.5 + 0.5;  // Oscillates between 0.0 and 1.0
        gradientColor = mix(textColorStart, textColorEnd, oscillation);
    }
    
    // Apply the gradient color to the text
    color = vec4(gradientColor, sampled.a) * sampled;
}