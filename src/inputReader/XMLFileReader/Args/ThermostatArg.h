#pragma once

#include <iostream>

class ThermostatArg {
 private:
  double initialTemp;
  double targetTemp;
  double maxTempChange;
  int periodLength;
  int dimension;

 public:
  ThermostatArg(double initialTemp, double targetTemp, double maxTempChange, int periodLength, int dimension);

  [[nodiscard]] double getInitialTemp() const;

  [[nodiscard]] double getTargetTemp() const;

  [[nodiscard]] double getMaxTempChange() const;

  [[nodiscard]] int getPeriodLength() const;

  [[nodiscard]] int getDimension() const;
};