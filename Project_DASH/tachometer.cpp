#include "tachometer.h"

Tachometer::Tachometer(unsigned int circleShader, unsigned int ticksShader, unsigned int needleShader, unsigned int textShader)
{
    this->circleShader = circleShader;
    this->ticksShader = ticksShader;
    this->needleShader = needleShader;
    this->textShader = textShader;

    this->cOuter = Circle(100, 0.4f, false);
    this->cInner = Circle(100, 0.395f, true);

    // Generate tick marks
    this->numMajorTicks = 18;
    this->tickOuterRadius = 0.387f; // Outer radius for tick marks
    this->tickInnerRadius = 0.32f;  // Inner radius for tick marks
    this->tickInnerRadiusSmall = 0.355f;  // Inner radius for tick marks
}

// ++++++++++++++++++++++++++++++++++++ CIRCLES
void Tachometer::createCircles() {
    this->circleVertices.push_back(glm::vec2(0.75f, 0.0f)); // center of the circle
    
    for (int i = 0; i <= this->cOuter.getNumSegments(); ++i) {
        float angle = i * 2.0f * 3.14159265359 / this->cOuter.getNumSegments();
        float x = this->cOuter.getRadius() * cos(angle) + 0.75f; // X coordinate
        float y = this->cOuter.getRadius() * sin(angle); // Y coordinate
        circleVertices.push_back(glm::vec2(x, y));
    }

    std::vector<glm::vec2> innerCircleVertices;
    innerCircleVertices.push_back(glm::vec2(0.75f, 0.0f)); // center of the small circle

    for (int i = 0; i <= this->cInner.getNumSegments(); ++i) {
        float angle = i * 2.0f * 3.14159265359 / this->cInner.getNumSegments();
        float x = this->cInner.getRadius() * cos(angle) + 0.75f; // X coordinate
        float y = this->cInner.getRadius() * sin(angle); // Y coordinate
        innerCircleVertices.push_back(glm::vec2(x, y));
    }

    this->circleVertices.insert(this->circleVertices.end(), innerCircleVertices.begin() + 1, innerCircleVertices.end()); // Skip the center point
}

std::vector<glm::vec2> Tachometer::getCircleVertices() {
    return this->circleVertices;
}

// ++++++++++++++++++++++++++++++++++++ TICKS

void Tachometer::createTicks() {
    for (int i = 0; i <= this->numMajorTicks; i++) {
        float angle = glm::radians(210.0f - i * (240.0f / this->numMajorTicks));
        float xOuter = this->tickOuterRadius * cos(angle) + 0.75f;
        float yOuter = this->tickOuterRadius * sin(angle);
        this->tickVertices.push_back(glm::vec2(xOuter, yOuter)); // Outer point

        if (i % 2 == 0) {
            float xInner = this->tickInnerRadius * cos(angle) + 0.75f;
            float yInner = this->tickInnerRadius * sin(angle);
            this->tickVertices.push_back(glm::vec2(xInner, yInner)); // Inner point
        }
        else {
            float xInner = this->tickInnerRadiusSmall * cos(angle) + 0.75f;
            float yInner = this->tickInnerRadiusSmall * sin(angle);
            this->tickVertices.push_back(glm::vec2(xInner, yInner)); // Inner point
        }
    }
}

std::vector<glm::vec2> Tachometer::getTickVertices() {
    return this->tickVertices;
}

// ++++++++++++++++++++++++++++++++++++ NEEDLE

void Tachometer::createNeedle() {
    // Needle vertices (one line from the center outward)
    this->needleVertices = {
        0.0f, 0.0f, 0.0f,  // Center of the gauge (origin)
        0.0f, 0.35f, 0.0f   // Needle tip
    };

    //this->needleVertices = {
    //    0.0f,  0.0f, 0.0f,  // Base center
    //    0.02f, -0.02f, 0.0f, // One side of the base (thickness)
    //    0.0f,  0.35f, 0.0f   // Needle tip
    //};
}

std::vector<float> Tachometer::getNeedleVertices() {
    return this->needleVertices;
}

// ++++++++++++++++++++++++++++++++++++

unsigned int Tachometer::getCircleShader() {
    return this->circleShader;
}

unsigned int Tachometer::getTicksShader() {
    return this->ticksShader;
}

unsigned int Tachometer::getNeedleShader() {
    return this->needleShader;
}

unsigned int Tachometer::getTextShader() {
    return this->textShader;
}

Circle Tachometer::getCircleOuter() {
    return this->cOuter;
}

Circle Tachometer::getCircleInner() {
    return this->cInner;
}

Tachometer::~Tachometer()
{
}