#include "Blinkers.h"

Blinkers::Blinkers(unsigned int blinkersShader) {
	this->blinkersShader = blinkersShader;
}

void Blinkers::createBlinkersVertices() {
    this->blinkersLeftVertices = {
        // Triangle 1
         0.05f, -0.4f,
         0.15f, -0.425f,
         0.15f, -0.375f,
    };

    this->blinkersRightVertices = {
        // Triangle 2      
         -0.4f, -0.37f,
         -0.4f, -0.42f,
         -0.3f,  -0.39f,
    };

    //this->blinkersVertices = {
    //    // Triangle 1
    //    -0.1f, -0.1f,
    //     0.1f, -0.1f,
    //    -0.1f,  0.1f,
    //    
    //    // Triangle 2
    //     0.1f, -0.1f,
    //     0.1f,  0.1f,
    //    -0.1f,  0.1f,
    //};
}

unsigned int Blinkers::getBlinkersShader() {
	return this->blinkersShader;
}

//std::vector<float> Blinkers::getBlinkersVertices() {
//	return this->blinkersVertices;
//}

std::vector<float> Blinkers::getLeftBlinkersVertices() {
	return this->blinkersLeftVertices;
}

std::vector<float> Blinkers::getRightBlinkersVertices() {
	return this->blinkersRightVertices;
}

Blinkers::~Blinkers() {

}