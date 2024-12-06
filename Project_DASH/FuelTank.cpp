#include "FuelTank.h"

FuelTank::FuelTank() {
    this->fuelAmountShader = 0;

    this->maxFuelAmount = 0.0f;
    this->fuelAmount = 0.0f;
}

FuelTank::FuelTank(unsigned int fuelAmountShader, float maxFuelAmount) {
    this->fuelAmountShader = fuelAmountShader;

    this->maxFuelAmount = maxFuelAmount;
    this->fuelAmount = 0.0f;
}

void FuelTank::createFuelTankVertices() {
    //this->fuelTankVertices = {    
    //    -0.9f, -0.8f, 0.0f,  // Bottom left
    //    -0.7f, -0.8f, 0.0f,  // Bottom right
    //    -0.7f, -0.75f, 0.0f, // Top right
    //    -0.9f, -0.75f, 0.0f  // Top left
    //};

    this->fuelTankVertices = {
       -1.65f, -0.3f, 0.0f,       // Bottom left
       -1.447f, -0.3f, 0.0f,       // Bottom right
       -1.447f, -0.25, 0.0f,      // Top right
       -1.65f, -0.25f, 0.0f       // Top left
    };
}

std::vector<float> FuelTank::getFuelTankVertices() {
    return this->fuelTankVertices;
}

void FuelTank::updateFuelTankVertices(float fillRatio) {
    this->fuelTankVertices = {
       -1.65f, -0.3f, 0.0f,                          // Bottom left
       -1.65f + 0.2f * fillRatio, -0.3f, 0.0f,       // Bottom right
       -1.65f + 0.2f * fillRatio, -0.25f, 0.0f,      // Top right
       -1.65f, -0.25f, 0.0f                          // Top left
    };
}

unsigned int FuelTank::getFuelTankShader() {
    return this->fuelAmountShader;
}

FuelTank::~FuelTank() {

}