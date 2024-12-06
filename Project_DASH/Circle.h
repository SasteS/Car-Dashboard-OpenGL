#include <vector>
#include <glm/glm.hpp>

#pragma once

class Circle
{
public:
    Circle();
    Circle(int numSegments, float radius, bool useGradient);
    ~Circle();
    int getNumSegments();
    float getRadius();
    bool getUseGradient();

private:
    bool useGradient; // true or false to toggle gradient or solid color
    int numSegments;
    float radius; // Radius of the circle
};