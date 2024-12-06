#include <vector>

#pragma once

class Radio {
public:
	Radio(unsigned int radioShader);

	void createRadioVertices();

	unsigned int getRadioShader();
	bool getIsRadioOn();
	void setIsRadioOn(bool isOn);
	float getTextAlpha();
	float getTextPositionX();
	float getRadioScreenWidth();
	void setTextAlpha(float textAlpha);
	void setPositionX(float positionX);

	std::vector<float> getRadioLeftVertices();
	std::vector<float> getRadioRightVertices();
	std::vector<float> getRadioBackgroundVertices();
	std::vector<float> getRadioButtonVertices();

	~Radio();

private:
	unsigned int radioShader;

	bool isRadioOn;
	float textAlpha;
	float textPositionX;
	float radioScreenWidth;

	std::vector<float> radioLeftVertices;
	std::vector<float> radioRightVertices;
	std::vector<float> radioBackgroundVertices;
	std::vector<float> radioButtonVertices;
};