#include "LinkedCellsColouringArg.h"

LinkedCellsColouringArg::LinkedCellsColouringArg(double cellSize, const std::array<double, 3>& leftLowerBound,
                                                 const std::array<double, 3>& rightUpperBound)
    : cellSize(cellSize), leftLowerBound(leftLowerBound), rightUpperBound(rightUpperBound) {}

double LinkedCellsColouringArg::getCellSize() const { return cellSize; }

const std::array<double, 3>& LinkedCellsColouringArg::getLeftLowerBound() const { return leftLowerBound; }

const std::array<double, 3>& LinkedCellsColouringArg::getRightUpperBound() const { return rightUpperBound; }
