#include "Radio.h"

Radio::Radio(unsigned int radioShader) {
	this->radioShader = radioShader;

	this->isRadioOn = true;
	this->textAlpha = 0.0f; // For gradual text appearance
	this->textPositionX = 3000.0f; // Start position of the text
	this->radioScreenWidth = -1000.0f;
}

void Radio::createRadioVertices() {
    //this->radioLeftVertices = {
    //    -0.355f, -0.3f, 0.0f, // Bottom-left
    //    -0.32f, -0.3f, 0.0f, // Bottom-right
    //    -0.32f, 0.3f, 0.0f, // Top-right
    //    -0.355f, 0.3f, 0.0f  // Top-left
    //};

    //// Right rectangle vertices
    //this->radioRightVertices = {
    //    0.175f, -0.3f, 0.0f, // Bottom-left
    //    0.21f, -0.3f, 0.0f, // Bottom-right
    //    0.21f, 0.3f, 0.0f, // Top-right
    //    0.175f, 0.3f, 0.0f  // Top-left
    //};

    //// Background rectangle vertices
    //this->radioBackgroundVertices = {
    //   -0.32f, -0.29f, 0.0f, // Bottom-left
    //    0.175f, -0.29f, 0.0f, // Bottom-right
    //    0.175f, 0.3f, 0.0f, // Top-right
    //   -0.32f, 0.3f, 0.0f  // Top-left
    //};
    
    // Left rectangle vertices with colors
    this->radioLeftVertices = {
        -0.355f, -0.3f, 0.0f,  0.02f, 0.0f, 0.02f, // Bottom-left 
        -0.32f, -0.3f, 0.0f,   0.02f, 0.0f, 0.02f, // Bottom-right
        -0.32f, 0.3f, 0.0f,    0.02f, 0.0f, 0.02f, // Top-right
        -0.355f, 0.3f, 0.0f,   0.02f, 0.0f, 0.02f  // Top-left 
    };

    // Right rectangle vertices with colors
    this->radioRightVertices = {
        0.175f, -0.3f, 0.0f,   0.02f, 0.0f, 0.02f, // Bottom-left 
        0.21f, -0.3f, 0.0f,    0.02f, 0.0f, 0.02f, // Bottom-right
        0.21f, 0.3f, 0.0f,     0.02f, 0.0f, 0.02f, // Top-right
        0.175f, 0.3f, 0.0f,    0.02f, 0.0f, 0.02f  // Top-left
    };

    // Background rectangle vertices with gradient fading inward
    this->radioBackgroundVertices = {
        -0.32f, -0.29f, 0.0f,  0.02f, 0.0f, 0.04f, // Bottom-left
         0.175f, -0.29f, 0.0f, 0.02f, 0.0f, 0.04f, // Bottom-right
         0.175f, 0.3f, 0.0f,  0.3f, 0.0f, 0.4f, // Top-right 
        -0.32f, 0.3f, 0.0f,   0.3f, 0.0f, 0.4f  // Top-left 
    };

    // Background rectangle vertices with gradient fading inward
    this->radioButtonVertices = {
        -0.11f, 0.4f, 0.0f,  0.4f, 0.4f, 0.4f, // Bottom-left 
         0.0f, 0.4f, 0.0f, 0.4f, 0.4f, 0.4f, // Bottom-right
         0.0f, 0.57f, 0.0f,  0.2f, 0.2f, 0.2f, // Top-right 
        -0.11f, 0.57f, 0.0f,   0.2f, 0.2f, 0.2f  // Top-left 
    };
}

std::vector<float> Radio::getRadioLeftVertices() {
    return this->radioLeftVertices;
}

std::vector<float> Radio::getRadioRightVertices() {
    return this->radioRightVertices;
}

std::vector<float> Radio::getRadioBackgroundVertices() {
    return this->radioBackgroundVertices;
}

std::vector<float> Radio::getRadioButtonVertices() {
    return this->radioButtonVertices;
}

unsigned int Radio::getRadioShader() {
	return this->radioShader;
}

bool Radio::getIsRadioOn() {
	return this->isRadioOn;
}

float Radio::getTextAlpha() {
	return this->textAlpha;
}

float Radio::getTextPositionX() {
	return this->textPositionX;
}

float Radio::getRadioScreenWidth() {
	return this->radioScreenWidth;
}

void Radio::setTextAlpha(float textAlpha) {
	this->textAlpha = textAlpha;
}

void Radio::setPositionX(float positionX) {
	this->textPositionX = positionX;
}

void Radio::setIsRadioOn(bool isOn) {
    this->isRadioOn = isOn;
}

Radio::~Radio() {

}