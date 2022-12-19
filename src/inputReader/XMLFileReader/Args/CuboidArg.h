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
  CuboidArg(const std::array<double, 3>& position,
            const std::array<double, 3>& velocity,
            const std::array<int, 3>& dimension, double distance, double mass,
            int type);

  [[nodiscard]] const std::array<double, 3>& getPosition() const;

  [[nodiscard]] const std::array<double, 3>& getVelocity() const;

  [[nodiscard]] const std::array<int, 3>& getDimension() const;

  [[nodiscard]] double getDistance() const;

  [[nodiscard]] double getMass() const;

  [[nodiscard]] int getType() const;
};
