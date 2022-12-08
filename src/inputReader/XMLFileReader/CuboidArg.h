#pragma once

#include <array>
#include <iostream>

class CuboidArg {
 private:
  std::array<double, 3> position;
  std::array<double, 3> velocity;
  std::array<int, 3> dimension;
  double distance;
  double mass;
  int type;

 public:
  CuboidArg(std::array<double, 3> position, std::array<double, 3> velocity,
            std::array<int, 3> dimension, double distance, double mass,
            int type)
      : position{position},
        velocity{velocity},
        dimension{dimension},
        distance{distance},
        mass{mass},
        type{type} {}

  std::array<double, 3> getPosition() { return position; }

  std::array<double, 3> getVelocity() { return velocity; }

  std::array<int, 3> getDimension() { return dimension; }

  double getDistance() { return distance; }

  double getMass() { return mass; }

  int getType() { return type; }
};
