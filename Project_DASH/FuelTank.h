#include <vector>

#pragma once

class FuelTank {
public:
	FuelTank();
	FuelTank(unsigned int fuelAmountShader, float maxFuelAmount);
	~FuelTank();

	void createFuelTankVertices();
	void updateFuelTankVertices(float fillRatio);
	std::vector<float> getFuelTankVertices();

	unsigned int getFuelTankShader();

private:
	unsigned int fuelAmountShader;

	float maxFuelAmount;
	float fuelAmount;
	std::vector<float> fuelTankVertices;
};