#pragma once

#include <array>
#include <iostream>

class LinkedCellArg {
 private:
  double cellSize;
  double cutOffRadius;
  std::array<double, 3> leftLowerBound;
  std::array<double, 3> rightUpperBound;

 public:
  LinkedCellArg(double cellSize, double cutOffRadius,
                const std::array<double, 3> &leftLowerBound,
                const std::array<double, 3> &rightUpperBound);
  double getCellSize() const;

  double getCutOffRadius() const;

  const std::array<double, 3> &getLeftLowerBound() const;
  const std::array<double, 3> &getRightUpperBound() const;
};