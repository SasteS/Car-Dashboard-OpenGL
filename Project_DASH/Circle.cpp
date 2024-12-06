#include "Circle.h"

Circle::Circle() {
    this->numSegments = 100;
    this->radius = 0;
    this->useGradient = false;
}

Circle::Circle(int numSegments, float radius, bool useGradient)
{
    this->numSegments = numSegments;
    this->radius = radius;
    this->useGradient = useGradient;
}

int Circle::getNumSegments() {
    return this->numSegments;
}

float Circle::getRadius() {
    return this->radius;
}

bool Circle::getUseGradient() {
    return this->useGradient;
}

Circle::~Circle()
{
}