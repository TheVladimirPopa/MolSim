#include "Thermostat.h"

#include <cmath>

#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

Thermostat::Thermostat(IContainer &container, double initialTemp, double targetTemp, double maxTempChange,
                       int periodLength_, size_t dimensionCount_)
    : particleContainer{container},
      initialTemperature{initialTemp},
      targetTemperature{targetTemp},
      maxTemperatureChange{maxTempChange},
      periodLength{periodLength_},
      dimensionCount{dimensionCount_} {}

void Thermostat::applyThermostat() {
  double curTemp = getCurrentTemperature();
  double temperatureDifference = targetTemperature - curTemp;
  double currentTarget = targetTemperature;

  // Make sure the change is smaller the maxTemperatureChange
  if (temperatureDifference > maxTemperatureChange) currentTarget = curTemp + maxTemperatureChange;

  if (temperatureDifference < -maxTemperatureChange) currentTarget = curTemp - maxTemperatureChange;

  setTemperature(curTemp, currentTarget);
}

void Thermostat::initializeTemperature() {
  // Test if all velocities are 0
  bool allZero = true;
  std::array<double, 3> zeroVector{0., 0., 0.};
  std::function<void(Particle &)> allVelocitiesZero = [&allZero, &zeroVector](Particle &p) {
    if (allZero && !(p.v == zeroVector)) {
      allZero = false;
    }
  };
  particleContainer.forEach(allVelocitiesZero);

  if (allZero) {
    // If all velocities are zero, initialize the velocities with brownian
    // motion
    auto initTemp = initialTemperature;
    auto dimCount = dimensionCount;
    std::function<void(Particle &)> applyBrownianMotion = [initTemp, dimCount](Particle &p) {
      double meanVel = std::sqrt(initTemp / p.m);
      p.v = maxwellBoltzmannDistributedVelocity(meanVel, dimCount);
    };
    particleContainer.forEach(applyBrownianMotion);
  }

  setTemperature(getCurrentTemperature(), initialTemperature);
}

void Thermostat::setTemperature(double currentTemperature, double newTemperature) {
  spdlog::debug("Set temperature to {} from {}", newTemperature, currentTemperature);

  double scaleFactor = sqrt(newTemperature / currentTemperature);
  std::function<void(Particle &)> scaleVelocity = [scaleFactor](Particle &p) { p.v = scaleFactor * p.v; };

  particleContainer.forEach(scaleVelocity);
}

double Thermostat::computeEnergy() {
  double energy{0.};
  std::function<void(Particle &)> energyCalc = [&energy](Particle &p) { energy += (p.m * ArrayUtils::dotProduct(p.v)) / 2.; };
  particleContainer.forEach(energyCalc);

  return energy;
}

double Thermostat::getCurrentTemperature() {
  return (computeEnergy() * 2.) / static_cast<double>(particleContainer.size() * dimensionCount);
}

int Thermostat::getPeriodLength() const { return periodLength; }
double Thermostat::getInitialTemperature() const { return initialTemperature; }
double Thermostat::getTargetTemperature() const { return targetTemperature; }
double Thermostat::getMaxTemperatureChange() const { return maxTemperatureChange; }
size_t Thermostat::getDimensionCount() const { return dimensionCount; }
