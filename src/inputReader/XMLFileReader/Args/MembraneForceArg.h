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

  int getRow() const;

  int getColumn() const;

  double getX() const;

  double getY() const;

  double getZ() const;

  int getTimeSpan() const;
};
