#pragma once

#include "dataStructures/IContainer.h"
class Thermostat {
 private:
  IContainer &particleContainer;

  double initialTemperature;

  double targetTemperature;

  double maxTemperatureChange;

  int periodLength;

 public:
  int getPeriodLength() const;

 private:
  size_t dimensionCount;

 public:
  Thermostat(IContainer &container, double initialTemp, double targetTemp,
             double maxTempChange, int periodLength_,
             size_t dimensionCount_ = 3);

  void initializeTemperature();

  void applyThermostat();

  double computeEnergy();

  double getCurrentTemperature();
};
