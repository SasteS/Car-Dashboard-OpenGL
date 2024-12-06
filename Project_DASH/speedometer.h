#include "Circle.h"

#pragma once

class Speedometer
{
public:
	Speedometer(unsigned int circleShader, unsigned int ticksShader, unsigned int needleShader, unsigned int textShader);
	~Speedometer();

	unsigned int getCircleShader();
	unsigned int getTicksShader();
	unsigned int getNeedleShader();
	unsigned int getTextShader();

	void createCircles();
	Circle getCircleOuter();
	Circle getCircleInner();
	std::vector<glm::vec2> getCircleVertices();

	void createTicks();
	std::vector<glm::vec2> getTickVertices();

	void createNeedle();
	std::vector<float> getNeedleVertices();

private:
	unsigned int circleShader;
	unsigned int ticksShader;
	unsigned int needleShader;
	unsigned int textShader;

	// CIRCLE
	Circle cOuter;
	Circle cInner;
	std::vector<glm::vec2> circleVertices;

	// TICKS
	int numMajorTicks; // Adjust for the number of major speed divisions
	float tickOuterRadius; // Outer radius for tick marks
	float tickInnerRadius;  // Inner radius for tick marks
	float tickInnerRadiusSmall;  // Inner radius for tick marks
	std::vector<glm::vec2> tickVertices;
	
	// NEEDLE
	std::vector<float> needleVertices;
};