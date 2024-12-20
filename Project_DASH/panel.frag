#version 330 core

out vec4 FragColor;

uniform mat4 projection;
uniform vec2 resolution;
uniform bool isVisor;

void main()
{
    // Normalize the fragment coordinates to the range [-1, 1]
    vec2 fragCoord = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;

    // Adjust for aspect ratio
    fragCoord.x *= resolution.x / resolution.y;

    // Calculate the top-to-bottom gradient using the normalized y-coordinate
    float distTopToBottom = (1.0 - (fragCoord.y + 1.0) / 2.0);  // Normalize y to [0, 1]

     vec3 colorTop;
     vec3 colorBottom;
    if (isVisor) {
        // Two colors for the gradient
        colorTop = vec3(0.0, 1.0, 1.0);
        colorBottom = vec3(0.0, 0.0, 0.0);
    }
    else {
        // Two colors for the gradient
        colorTop = vec3(0.2, 0.3, 0.8);
        colorBottom = vec3(0.8, 0.2, 0.2);
    }

    // Interpolate between the top and bottom color based on the y position
    vec3 color = mix(colorTop, colorBottom, distTopToBottom);

    FragColor = vec4(color, 0.2f);
}
