#version 330 core

in vec2 fragPos; // Interpolated position from vertex shader
out vec4 FragColor;

uniform vec2 circleCenter;    // Center of the circle
uniform float radius;         // Radius of the circle
uniform float temperatureRatio; // Temperature ratio (0 = fully blue, 1 = fully red)

void main()
{
    // Adjust fragment position relative to the circle center
    vec2 adjustedPos = fragPos - circleCenter;

    // Compute the distance from the center of the circle
    float distance = length(adjustedPos);

    // Discard fragments outside the circle
    if (distance > radius)
        discard;

    // Compute the threshold for red/blue transition based on temperature
    float threshold = mix(radius, -radius, temperatureRatio); // Invert the range to fill from top

    // Determine color based on the adjusted y-coordinate and threshold
    vec3 color;
    if (adjustedPos.y >= threshold) {
        color = vec3(1.0, 0.0, 0.0); // Red (hot)
    } else {
        color = vec3(0.0, 0.0, 1.0); // Blue (cold)
    }

    // Set the fragment color
    FragColor = vec4(color, 1.0);
}
