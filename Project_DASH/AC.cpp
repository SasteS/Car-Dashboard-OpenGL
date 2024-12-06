#include "AC.h"

AC::AC(unsigned int acShader, unsigned int textShader) {
	this->acShader = acShader;
	this->textShader = textShader;

	this->c = Circle(100, 0.2f, false);

	// State variables
	this->currentTemperature = 20.0f;
	this->minTemperature = 15.0f;
	this->maxTemperature = 30.0f;
	this->temperatureRatio = 0.5f;
}

void AC::createCircleVertices() {
	this->circleVertices.push_back(glm::vec2(-0.125f, -0.5f)); // center of the circle

	// Adding the points on the circumference of the large circle
	for (int i = 0; i <= this->c.getNumSegments(); ++i) {
		float angle = i * 2.0f * 3.14159265359 / this->c.getNumSegments(); // Calculating angle
		float x = this->c.getRadius() * cos(angle) - 0.125f; // X coordinate
		float y = this->c.getRadius() * sin(angle) - 0.5; // Y coordinate
		circleVertices.push_back(glm::vec2(x, y));
	}
}

unsigned int AC::getAcShader() {
	return this->acShader;
}

unsigned int AC::getTextShader() {
	return this->textShader;
}

Circle AC::getCircle() {
	return this->c;
}

std::vector<glm::vec2> AC::getCircleVertices() {
	return this->circleVertices;
}

float AC::getCurrentTemperature() {
	return this->currentTemperature;
}

void AC::setCurrentTemperature(float currentTemperature) {
	this->currentTemperature = currentTemperature;
}

float AC::getMinTemperature() {
	return this->minTemperature;
}

float AC::getMaxTemperature() {
	return this->maxTemperature;
}

float AC::getTemperatureRatio() {
	return this->temperatureRatio;
}

void AC::mapTemperatureToRatio() {
	this->temperatureRatio = (this->currentTemperature - this->minTemperature) / (this->maxTemperature - this->minTemperature);
}

AC::~AC() {

}