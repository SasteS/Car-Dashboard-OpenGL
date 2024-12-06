#version 330 core

uniform vec2 resolution;  // Screen resolution
uniform vec2 circleCenter;  // Center of the circle in normalized space
uniform bool useGradient;  // To toggle gradient effect
uniform float radius;      // The radius of the circle

out vec4 FragColor;  // Output color of the fragment

void main()
{
    // Normalize the fragment coordinates to the range [-1, 1]
    vec2 fragCoord = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;

    // Adjust for aspect ratio
    fragCoord.x *= resolution.x / resolution.y;

    // Shift to circle center
    vec2 distVec = fragCoord - circleCenter;

    // Calculate the distance from the center of the circle
    float distFromCenter = length(distVec);

    // Reverse the gradient direction: Bottom (0) to Top (1)
    float distTopToBottom = (1.0 - (fragCoord.y + 1.0) / 2.0);

    float steepnessFactor = 1.0;
    if (useGradient)
    {
        if (distFromCenter < radius)
        {
            steepnessFactor = 0.665;
            float gradient = smoothstep(0.0, 1.0, distFromCenter / radius * steepnessFactor);

            vec3 color = mix(vec3(0.02, 0.0, 0.02), vec3(0.3, 0.0, 0.3), gradient);
            color = mix(color, vec3(0.02, 0.0, 0.02), distTopToBottom / steepnessFactor);
            FragColor = vec4(color, 1.0);
        }
        else
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
    else
    {
        steepnessFactor = 1.8;
        vec3 color = mix(vec3(0.5, 0.0, 0.25), vec3(0.1, 0.0, 0.1), distTopToBottom * steepnessFactor);
        FragColor = vec4(color, 1.0);
    }
}
