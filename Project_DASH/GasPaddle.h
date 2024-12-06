#include <vector>

#pragma once

class GasPaddle {
public:
	GasPaddle();
	GasPaddle(unsigned int progressBarShader, float gasIncrement, float gasDecrement);
	~GasPaddle();

	void createProgressBar();
	void updateProgressBar();
	std::vector<float> getProgressBarVertices();

	unsigned int getProgressBarShader();

	float getGasLevel();
	void setGasLevel(float gasLevel);
	float getGasIncrement();
	float getGasDecrement();

private:
	unsigned int progressBarShader;

	float gasLevel;
	float gasIncrement;
	float gasDecrement;
	std::vector<float> progressBarVertices;
};