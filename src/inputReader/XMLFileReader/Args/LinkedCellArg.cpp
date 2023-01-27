#include "LinkedCellArg.h"

#include <utility>

LinkedCellArg::LinkedCellArg(double cellSize, const std::array<double, 3> &leftLowerBound,
                             const std::array<double, 3> &rightUpperBound, std::string parallelization)
    : cellSize(cellSize),
      leftLowerBound(leftLowerBound),
      rightUpperBound(rightUpperBound),
      parallelization{std::move(parallelization)} {}

double LinkedCellArg::getCellSize() const { return cellSize; }

const std::array<double, 3> &LinkedCellArg::getLeftLowerBound() const { return leftLowerBound; }

const std::array<double, 3> &LinkedCellArg::getRightUpperBound() const { return rightUpperBound; }

const std::string &LinkedCellArg::getBoundLeft() const { return bound_left; }

void LinkedCellArg::setBoundLeft(const std::string &boundLeft) { bound_left = boundLeft; }

const std::string &LinkedCellArg::getBoundRight() const { return bound_right; }

void LinkedCellArg::setBoundRight(const std::string &boundRight) { bound_right = boundRight; }

const std::string &LinkedCellArg::getBoundTop() const { return bound_top; }

void LinkedCellArg::setBoundTop(const std::string &boundTop) { bound_top = boundTop; }

const std::string &LinkedCellArg::getBoundBottom() const { return bound_bottom; }

void LinkedCellArg::setBoundBottom(const std::string &boundBottom) { bound_bottom = boundBottom; }

const std::string &LinkedCellArg::getBoundFront() const { return bound_front; }

void LinkedCellArg::setBoundFront(const std::string &boundFront) { bound_front = boundFront; }

const std::string &LinkedCellArg::getBoundBack() const { return bound_back; }

void LinkedCellArg::setBoundBack(const std::string &boundBack) { bound_back = boundBack; }

const std::string &LinkedCellArg::getParallelization() const { return parallelization; }
