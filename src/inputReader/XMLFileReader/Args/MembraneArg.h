#pragma once

#include <array>
#include <iostream>
#include <vector>

#include "MembraneForceArg.h"

class MembraneArg {
 private:
  std::array<double, 3> position;
  std::array<int, 3> dimension;
  std::array<double, 3> velocity;
  std::vector<MembraneForceArg> membraneForces;
  double distance;
  double mass;
  double stiffness;
  double bondLength;
  double cutOffRadius;
  int type;

 public:
  MembraneArg(const std::array<double, 3>& position, const std::array<int, 3>& dimension,
              const std::array<double, 3>& velocity, std::vector<MembraneForceArg> membraneForces, double distance,
              double mass, double stiffness, double bondLength, double cutOffRadius, int type);

  [[nodiscard]] const std::array<double, 3>& getPosition() const;

  [[nodiscard]] const std::array<int, 3>& getDimension() const;

  [[nodiscard]] const std::array<double, 3>& getVelocity() const;

  [[nodiscard]] const std::vector<MembraneForceArg>& getMembraneForces() const;

  [[nodiscard]] double getDistance() const;

  [[nodiscard]] double getMass() const;

  [[nodiscard]] double getStiffness() const;

  [[nodiscard]] double getBondLength() const;

  [[nodiscard]] double getCutOffRadius() const;

  [[nodiscard]] int getType() const;
};
