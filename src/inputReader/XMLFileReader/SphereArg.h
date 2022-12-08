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
  SphereArg(std::array<double, 3> position, std::array<double, 3> velocity,
            double radius, int dimension, double distance, double mass,
            int type)
      : position{position},
        velocity{velocity},
        radius{radius},
        dimension{dimension},
        distance{distance},
        mass{mass},
        type{type} {}

  std::array<double, 3> getPosition() { return position; }

  std::array<double, 3> getVelocity() { return velocity; }

  double getRadius() { return radius; }

  int getDimension() { return dimension; }

  double getDistance() { return distance; }

  double getMass() { return mass; }

  int getType() { return type; }
};
