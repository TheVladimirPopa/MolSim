#pragma once

#include <array>
#include <iostream>

class SphereArg {
 private:
  std::array<double, 3> position;
  std::array<double, 3> velocity;
  unsigned int radius;
  unsigned int dimension;
  double distance;
  double mass;
  int type;

 public:
  SphereArg(const std::array<double, 3>& position, const std::array<double, 3>& velocity, unsigned int radius,
            unsigned int dimension, double distance, double mass, int type);

  [[nodiscard]] const std::array<double, 3>& getPosition() const;

  [[nodiscard]] const std::array<double, 3>& getVelocity() const;

  [[nodiscard]] unsigned int getRadius() const;

  [[nodiscard]] unsigned int getDimension() const;

  [[nodiscard]] double getDistance() const;

  [[nodiscard]] double getMass() const;

  [[nodiscard]] int getType() const;
};
