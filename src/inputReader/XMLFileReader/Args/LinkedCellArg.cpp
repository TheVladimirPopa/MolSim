#include "LinkedCellArg.h"

double LinkedCellArg::getCellSize() const { return cellSize; }

double LinkedCellArg::getCutOffRadius() const { return cutOffRadius; }
const std::array<double, 3> &LinkedCellArg::getLeftLowerBound() const {
  return leftLowerBound;
}
const std::array<double, 3> &LinkedCellArg::getRightUpperBound() const {
  return rightUpperBound;
}
LinkedCellArg::LinkedCellArg(double cellSize, double cutOffRadius,
                             const std::array<double, 3> &leftLowerBound,
                             const std::array<double, 3> &rightUpperBound)
    : cellSize(cellSize),
      cutOffRadius(cutOffRadius),
      leftLowerBound(leftLowerBound),
      rightUpperBound(rightUpperBound) {}
