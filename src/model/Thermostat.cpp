#include "Thermostat.h"

#include <cmath>

#include "utils/ArrayUtils.h"
Thermostat::Thermostat(IContainer &container, double initialTemp,
                       double targetTemp, double maxTempChange,
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
  if (temperatureDifference > maxTemperatureChange)
    currentTarget = curTemp + maxTemperatureChange;

  if (temperatureDifference < -maxTemperatureChange)
    currentTarget = curTemp - maxTemperatureChange;

  double scaleFactor = sqrt(currentTarget / curTemp);

  std::function scaleVelocity = [scaleFactor](Particle &p) {
    p.v = scaleFactor * p.v;
  };

  particleContainer.forEach(scaleVelocity);
}
double Thermostat::computeEnergy() {
  double energy{0.};
  std::function energyCalc = [&energy](Particle &p) {
    energy += (p.m * ArrayUtils::dotProduct(p.v)) / 2.;
  };
  particleContainer.forEach(energyCalc);

  return energy;
}
double Thermostat::getCurrentTemperature() {
  return (computeEnergy() * 2.) /
         static_cast<double>(particleContainer.size() * dimensionCount);
}
int Thermostat::getPeriodLength() const { return periodLength; }
