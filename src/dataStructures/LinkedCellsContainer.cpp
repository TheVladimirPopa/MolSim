//
// Created by leo on 26.11.22.
//

#include "LinkedCellsContainer.h"

#include "utils/ArrayUtils.h"

LinkedCellsContainer::LinkedCellsContainer(
    double cellSize, std::array<double, 3> &leftLowerBound,
    std::array<double, 3> &rightUpperBound)
    : gridSize{cellSize},
      leftLowerCorner{leftLowerBound},
      rightUpperCorner{rightUpperBound} {
  std::array<double, 3> boundingBoxDim{rightUpperCorner - leftLowerCorner};
  size_t numCells{1};
  for (int i = 0; i < 3; ++i) {
    dimensions[i] = static_cast<size_t>(ceil(boundingBoxDim[i] / gridSize)) + 2;
    numCells *= dimensions[i];
  }

  cells.reserve(numCells);

  for (size_t x = 0; x < dimensions[0]; ++x) {
    for (size_t y = 0; y < dimensions[1]; ++y) {
      for (size_t z = 0; z < dimensions[2]; ++z) {
        if (x == 0 or y == 0 or z == 0 or x == dimensions[0] - 1 or
            y == dimensions[1] - 1 or z == dimensions[2] - 1) {
          cells[x + (y * dimensions[0]) + (z * dimensions[0] * dimensions[1])] =
              cell{halo};
        } else if (x == 1 or y == 1 or z == 1 or x == dimensions[0] - 2 or
                   y == dimensions[1] - 2 or z == dimensions[2] - 2) {
          cells[getVectorIndexFromCord(x, y, z)] = cell{boundary};
        } else {
          cells[x + (y * dimensions[0]) + (z * dimensions[0] * dimensions[1])] =
              cell{inner};
        }
      }
    }
  }
}
size_t LinkedCellsContainer::getVectorIndexFromCord(size_t x, size_t y,
                                                    size_t z) {
  return x + (y * dimensions[0]) + (z * dimensions[0] * dimensions[1]);
}
size_t LinkedCellsContainer::getCellIndexOfPosition(
    std::array<double, 3> &position) {
  std::array<double, 3> positionInBox = position - leftLowerCorner;
  std::array<int, 3> indexInBox{};
  for (int i = 0; i < 3; ++i) {
    indexInBox[i] = static_cast<int>(floor(positionInBox[i] / gridSize));
    if (indexInBox[i] < 0) {
      indexInBox[i] = -1;
    }
    if (indexInBox[i] > dimensions[i]) {
      indexInBox[i] = dimensions[i];
    }

    indexInBox[i] += 1;
  }

  return getVectorIndexFromCord(indexInBox[0], indexInBox[1], indexInBox[2]);
}
void LinkedCellsContainer::forEach(
    std::function<void(Particle &)> &unaryFunction) {
  for (auto &p : particlesVector) {
    unaryFunction(p);
  }
}
void LinkedCellsContainer::forEachPair(
    std::function<void(Particle &, Particle &)> &binaryFunction) {}

void LinkedCellsContainer::reserve(size_t amount) {
  particlesVector.reserve(amount);
}
size_t LinkedCellsContainer::capacity() { return particlesVector.capacity(); }
size_t LinkedCellsContainer::size() { return particlesVector.size(); }
