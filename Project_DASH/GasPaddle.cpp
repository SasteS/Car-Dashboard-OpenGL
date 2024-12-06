#include "GasPaddle.h"

GasPaddle::GasPaddle() {
	this->progressBarShader = 0;

	this->gasLevel = 0.0f; // Range: 0.0 (empty) to 1.0 (full)
	this->gasIncrement = 0.01f; // Rate of increase
	this->gasDecrement = 0.005f; // Rate of decrease
}

GasPaddle::GasPaddle(unsigned int progressBarShader, float gasIncrement, float gasDecrement) {
	this->progressBarShader = progressBarShader;

	this->gasLevel = 0.0f;
	this->gasIncrement = gasIncrement; // Rate of increase
	this->gasDecrement = gasDecrement; // Rate of decrease
}

void GasPaddle::createProgressBar() {
	//this->progressBarVertices = {
	//-0.9f, -0.8f, 0.0f,  // Bottom-left corner
	//-0.7f, -0.8f, 0.0f,  // Bottom-right corner
	//-0.7f, -0.8f + gasLevel * 0.6f, 0.0f, // Top-right corner (dynamic)
	//-0.9f, -0.8f + gasLevel * 0.6f, 0.0f  // Top-left corner (dynamic)
	//};

	this->progressBarVertices = {
		// X, Y, Z
		1.5f, -0.3f, 0.0f,  // Bottom-left
		1.6f, -0.3f, 0.0f,  // Bottom-right
		1.6f,  0.2f, 0.0f,  // Top-right
		1.5f,  0.2f, 0.0f   // Top-left
	};
}

void GasPaddle::updateProgressBar() {
	this->progressBarVertices[7] = -0.3f + this->gasLevel * 0.6f; // Update top-right corner
	this->progressBarVertices[10] = -0.3f + this->gasLevel * 0.6f; // Update top-left corner
}

unsigned int GasPaddle::getProgressBarShader() {
	return this->progressBarShader;
}

std::vector<float> GasPaddle::getProgressBarVertices() {
	return this->progressBarVertices;
}

float GasPaddle::getGasLevel() {
	return this->gasLevel;
}

void GasPaddle::setGasLevel(float gasLevel) {
	this->gasLevel = gasLevel;
}

float GasPaddle::getGasIncrement() {
	return this->gasIncrement;
}

float GasPaddle::getGasDecrement() {
	return this->gasDecrement;
}

GasPaddle::~GasPaddle() {

}