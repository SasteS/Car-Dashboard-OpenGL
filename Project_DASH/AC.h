#include "Circle.h"

#pragma once

class AC {
public:
	AC(unsigned int acShader, unsigned int textShader);
	~AC();

	void createCircleVertices();

	unsigned int getAcShader();
	unsigned int getTextShader();
	Circle getCircle();
	std::vector<glm::vec2> getCircleVertices();
	float getCurrentTemperature();
	void setCurrentTemperature(float currentTemperature);
	float getMinTemperature();
	float getMaxTemperature();
	float getTemperatureRatio();
	// Utility function to map temperature to a ratio
	void mapTemperatureToRatio();

private:
	unsigned int acShader;
	unsigned int textShader;

	Circle c;

	float currentTemperature; // Initial temperature
	float minTemperature;     // Minimum temperature
	float maxTemperature;     // Maximum temperature
	float temperatureRatio;    // Ratio of red to blue (0.0 = 

	std::vector<glm::vec2> circleVertices;
};