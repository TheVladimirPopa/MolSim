#pragma once

#include "dataStructures/IContainer.h"
class Thermostat {
 private:
  /// The container the thermostat gets applied to
  IContainer &particleContainer;

  /// The initial temperature that is set, when invoking initializeTemperature()
  double initialTemperature;

  /// The temperature the thermostats targets
  double targetTemperature;

  /// The maximal absolute temperature change allowed for one application of
  /// the thermostat
  double maxTemperatureChange;

  /// The number of time steps after which the thermostat is periodically ap-
  /// plied
  int periodLength;

  /// How many dimensions the simulation has
  size_t dimensionCount;

  /**
   * Scales the velocities to match the new temperature
   * @param currentTemperature the current temperature of all particles
   * @param newTemperature the temperature that the particles should have
   */
  void setTemperature(double currentTemperature, double newTemperature);

 public:
  /**
   * Initialize a thermostat with a specific temperature for a container
   * @param container The container the thermostat gets applied to
   * @param initialTemp The initial temperature that is set, when invoking
   * initializeTemperature()
   * @param targetTemp The temperature the thermostats targets
   * @param maxTempChange The maximal absolute temperature change allowed for
   * one application of the thermostat
   * @param periodLength_ The number of time steps after which the thermostat is
   * periodically applied
   * @param dimensionCount_ How many dimensions the simulation has
   */
  Thermostat(IContainer &container, double initialTemp, double targetTemp, double maxTempChange, int periodLength_,
             size_t dimensionCount_ = 3);

  /**
   * Initializes the container with a specific temperature
   */
  void initializeTemperature();

  /**
   * Apply the thermostat to change the temperature towards the target
   * temperature
   */
  void applyThermostat();

  /**
   * Computes the energy of all particles
   * @return the current kinetic energy of all particles
   */
  double computeEnergy();

  /**
   * Returns the current temperature of the container
   * @return the current temperature of the container
   */
  double getCurrentTemperature();

  /**
   * Returns how many iterations are between two thermostat applications
   * @return how many iterations are between two thermostat applications
   */
  int getPeriodLength() const;
};
