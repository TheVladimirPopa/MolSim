#pragma once

#include "dataStructures/IContainer.h"
class Thermostat {
 private:
  IContainer &particleContainer;

  double initialTemperature;

  double targetTemperature;

  double maxTemperatureChange;

  int periodLength;

  size_t dimensionCount;

  void setTemperature(double currentTemperature, double newTemperature);

 public:
  Thermostat(IContainer &container, double initialTemp, double targetTemp,
             double maxTempChange, int periodLength_,
             size_t dimensionCount_ = 3);

  void initializeTemperature();

  void applyThermostat();

  double computeEnergy();

  double getCurrentTemperature();

  int getPeriodLength() const;
};
