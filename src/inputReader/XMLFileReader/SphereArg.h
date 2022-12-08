#pragma once

#include <array>
#include <iostream>

class SphereArg {
 private:
  std::array<double, 3> position;
  std::array<double, 3> velocity;
  double radius;
  int dimension;
  double distance;
  double mass;
  int type;

 public:
  SphereArg(const std::array<double, 3>& position,
            const std::array<double, 3>& velocity, double radius, int dimension,
            double distance, double mass, int type);

  const std::array<double, 3>& getPosition() const;

  const std::array<double, 3>& getVelocity() const;

  double getRadius() const;

  int getDimension() const;

  double getDistance() const;

  double getMass() const;

  int getType() const;

};
