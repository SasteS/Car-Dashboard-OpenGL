#include <vector>

#pragma once

class Blinkers {
public:
	//Blinkers();
	Blinkers(unsigned int blinkersShader);

	unsigned int getBlinkersShader();
	void createBlinkersVertices();
	std::vector<float> getLeftBlinkersVertices();
	std::vector<float> getRightBlinkersVertices();
	//std::vector<float> getBlinkersVertices();

	~Blinkers();

private:
	unsigned int blinkersShader;

	std::vector<float> blinkersLeftVertices;
	std::vector<float> blinkersRightVertices;
//	std::vector<float> blinkersVertices;
};