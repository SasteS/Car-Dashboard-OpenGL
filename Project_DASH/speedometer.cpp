#include "speedometer.h"

Speedometer::Speedometer(unsigned int circleShader, unsigned int ticksShader, unsigned int needleShader, unsigned int textShader)
{
	this->circleShader = circleShader;
	this->ticksShader = ticksShader;
	this->needleShader = needleShader;
	this->textShader = textShader;

	this->cOuter = Circle(100, 0.4f, false);
	this->cInner = Circle(100, 0.395f, true);

    // Generate tick marks
    this->numMajorTicks = 18; // Adjust for the number of major speed divisions
    this->tickOuterRadius = 0.387f; // Outer radius for tick marks
    this->tickInnerRadius = 0.32f;  // Inner radius for tick marks
    this->tickInnerRadiusSmall = 0.355f;  // Inner radius for tick marks
}

// ++++++++++++++++++++++++++++++++++++ CIRCLES
void Speedometer::createCircles() {
    this->circleVertices.push_back(glm::vec2(-1.0f, 0.0f)); // center of the circle

    // Add the points on the circumference of the large circle
    for (int i = 0; i <= this->cOuter.getNumSegments(); ++i) {
        float angle = i * 2.0f * 3.14159265359 / this->cOuter.getNumSegments(); // Calculate angle
        float x = this->cOuter.getRadius() * cos(angle) - 1.0f; // X coordinate
        float y = this->cOuter.getRadius() * sin(angle); // Y coordinate
        circleVertices.push_back(glm::vec2(x, y)); // Store the vertex
    }

    // Add the center of the small circle
    std::vector<glm::vec2> innerCircleVertices;
    innerCircleVertices.push_back(glm::vec2(-1.0f, 0.0f)); // center of the small circle

    // Add the points on the circumference of the small circle
    for (int i = 0; i <= this->cInner.getNumSegments(); ++i) {
        float angle = i * 2.0f * 3.14159265359 / this->cInner.getNumSegments(); // Calculate angle
        float x = this->cInner.getRadius() * cos(angle) - 1.0f; // X coordinate
        float y = this->cInner.getRadius() * sin(angle); // Y coordinate
        innerCircleVertices.push_back(glm::vec2(x, y)); // Store the vertex
    }

    // Combine both circle vertices into one
    this->circleVertices.insert(this->circleVertices.end(), innerCircleVertices.begin() + 1, innerCircleVertices.end()); // Skip the center point
}

std::vector<glm::vec2> Speedometer::getCircleVertices() {
    return this->circleVertices;
}

// ++++++++++++++++++++++++++++++++++++ TICKS

void Speedometer::createTicks() {
    for (int i = 0; i <= this->numMajorTicks; i++) {
        float angle = glm::radians(210.0f - i * (240.0f / this->numMajorTicks)); // 180 degrees for semicircle
        float xOuter = this->tickOuterRadius * cos(angle) - 1.0f;
        float yOuter = this->tickOuterRadius * sin(angle);
        this->tickVertices.push_back(glm::vec2(xOuter, yOuter)); // Outer point

        if (i % 2 == 0) {
            float xInner = this->tickInnerRadius * cos(angle) - 1.0f;
            float yInner = this->tickInnerRadius * sin(angle);
            this->tickVertices.push_back(glm::vec2(xInner, yInner)); // Inner point
        }
        else {
            float xInner = this->tickInnerRadiusSmall * cos(angle) - 1.0f;
            float yInner = this->tickInnerRadiusSmall * sin(angle);
            this->tickVertices.push_back(glm::vec2(xInner, yInner)); // Inner point
        }
    }
}

std::vector<glm::vec2> Speedometer::getTickVertices() {
    return this->tickVertices;
}

// ++++++++++++++++++++++++++++++++++++ NEEDLE

void Speedometer::createNeedle() {
    // Needle vertices (one line from the center outward)
    this->needleVertices = {
        0.0f, 0.0f, 0.0f,  // Center of the gauge (origin)
        0.0f, 0.35f, 0.0f   // Needle tip (this length will be adjusted based on your design)
    };
}

std::vector<float> Speedometer::getNeedleVertices() {
        return this->needleVertices;
}

// ++++++++++++++++++++++++++++++++++++

unsigned int Speedometer::getCircleShader() {
    return this->circleShader;
}

unsigned int Speedometer::getTicksShader() {
    return this->ticksShader;
}

unsigned int Speedometer::getNeedleShader() {
    return this->needleShader;
}

unsigned int Speedometer::getTextShader() {
    return this->textShader;
}

Circle Speedometer::getCircleOuter() {
    return this->cOuter;
}

Circle Speedometer::getCircleInner() {
    return this->cInner;
}

Speedometer::~Speedometer()
{
}