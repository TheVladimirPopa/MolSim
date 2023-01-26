#pragma once
#include <array>
#include <iostream>

class LinkedCellsColouringArg {
 private:
  double cellSize;
  std::array<double, 3> leftLowerBound;
  std::array<double, 3> rightUpperBound;

 public:
  LinkedCellsColouringArg(double cellSize, const std::array<double, 3>& leftLowerBound,
                          const std::array<double, 3>& rightUpperBound);

  [[nodiscard]] double getCellSize() const;

  [[nodiscard]] const std::array<double, 3>& getLeftLowerBound() const;

  [[nodiscard]] const std::array<double, 3>& getRightUpperBound() const;
};
