#include "MembraneForceArg.h"
MembraneForceArg::MembraneForceArg(int row, int column, double x, double y, double z, int timeSpan)
    : row(row), column(column), x(x), y(y), z(z), timeSpan(timeSpan) {}

int MembraneForceArg::getRow() const { return row; }

int MembraneForceArg::getColumn() const { return column; }

double MembraneForceArg::getX() const { return x; }

double MembraneForceArg::getY() const { return y; }

double MembraneForceArg::getZ() const { return z; }

int MembraneForceArg::getTimeSpan() const { return timeSpan; }
