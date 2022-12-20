#include "ThermostatArg.h"

ThermostatArg::ThermostatArg(double initialTemp, double targetTemp, double maxTempChange, int periodLength,
                             int dimension)
    : initialTemp(initialTemp),
      targetTemp(targetTemp),
      maxTempChange(maxTempChange),
      periodLength(periodLength),
      dimension(dimension) {}

double ThermostatArg::getInitialTemp() const { return initialTemp; }

double ThermostatArg::getTargetTemp() const { return targetTemp; }

double ThermostatArg::getMaxTempChange() const { return maxTempChange; }

int ThermostatArg::getPeriodLength() const { return periodLength; }

int ThermostatArg::getDimension() const { return dimension; }
