#pragma once
#include <iostream>

class MembraneForceArg {
 private:
  int row;
  int column;
  double x;
  double y;
  double z;
  int timeSpan;

 public:
  MembraneForceArg(int row, int column, double x, double y, double z, int timeSpan);

  [[nodiscard]] int getRow() const;

  [[nodiscard]] int getColumn() const;

  [[nodiscard]] double getX() const;

  [[nodiscard]] double getY() const;

  [[nodiscard]] double getZ() const;

  [[nodiscard]] int getTimeSpan() const;
};
