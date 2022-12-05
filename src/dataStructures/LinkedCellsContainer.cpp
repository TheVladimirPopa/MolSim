#include "LinkedCellsContainer.h"

#include "utils/ArrayUtils.h"

LinkedCellsContainer::LinkedCellsContainer(
    double cellSize, std::array<double, 3> &leftLowerBound,
    std::array<double, 3> &rightUpperBound)
    : cells{},
      gridSize{cellSize},
      leftLowerCorner{leftLowerBound},
      rightUpperCorner{rightUpperBound} {
  std::array<double, 3> boundingBoxDim{rightUpperCorner - leftLowerCorner};
  size_t numCells{1};
  for (int i = 0; i < 3; ++i) {
    dimensions[i] = static_cast<size_t>(ceil(boundingBoxDim[i] / gridSize)) + 2;
    numCells *= dimensions[i];
  }

  auto getBoundaries = [](size_t x, size_t y, size_t z,
                          std::array<unsigned, 3> &dimensions) {
    std::vector<cubeSide> boundaries{};

    if (x == 1) boundaries.emplace_back(cubeSide::LEFT);
    if (y == 1) boundaries.emplace_back(cubeSide::BOTTOM);
    if (z == 1) boundaries.emplace_back(cubeSide::BACK);

    if (x == dimensions[0] - 2) boundaries.emplace_back(cubeSide::RIGHT);
    if (y == dimensions[1] - 2) boundaries.emplace_back(cubeSide::TOP);
    if (z == dimensions[2] - 2) boundaries.emplace_back(cubeSide::FRONT);

    return boundaries;
  };

  cells.reserve(numCells);

  long index = 0;
  // Instantiate all the cells with their specified type
  for (size_t z = 0; z < dimensions[2]; ++z) {
    for (size_t y = 0; y < dimensions[1]; ++y) {
      for (size_t x = 0; x < dimensions[0]; ++x) {
        if (x == 0 or y == 0 or z == 0 or x == dimensions[0] - 1 or
            y == dimensions[1] - 1 or z == dimensions[2] - 1) {
          cells.emplace(cells.begin() + index, cellType::halo, index);
        } else if (x == 1 or y == 1 or z == 1 or x == dimensions[0] - 2 or
                   y == dimensions[1] - 2 or z == dimensions[2] - 2) {
          auto boundaries = getBoundaries(x, y, z, dimensions);
          boundaryCell boundary_cell(index, dimensions, boundaries);
          cells.push_back(std::move(boundary_cell));
        } else {
          cells.emplace(cells.begin() + index, cellType::inner, index);
        }
        ++index;
      }
    }
  }

  // Store boundary cells separately for easier handling
  for (auto &cell : cells) {
    if (cell.type == cellType::boundary) {
      boundaryCell bCell = static_cast<boundaryCell>(bCell);
      boundaryCells.push_back(&bCell);
    }
  }


  // Precompute the indexOffsets
  int indexAdjacentArray = 0;
  indexOffsetAdjacent[indexAdjacentArray++] = 0;
  indexOffsetAdjacent[indexAdjacentArray++] = 1;
  for (int x = -1; x < 2; ++x) {
    indexOffsetAdjacent[indexAdjacentArray++] = dimensions[0] + x;
  }
  for (int y = -1; y < 2; ++y) {
    for (int x = -1; x < 2; ++x) {
      indexOffsetAdjacent[indexAdjacentArray++] =
          static_cast<int>(dimensions[1] * dimensions[0]) + x +
          (y * dimensions[0]);
    }
  }
}

size_t LinkedCellsContainer::getVectorIndexFromCord(size_t x, size_t y,
                                                    size_t z) {
  return x + (y * dimensions[0]) + (z * dimensions[0] * dimensions[1]);
}

std::array<unsigned int, 3> LinkedCellsContainer::getCoordFromVectorIndexStatic(
    unsigned int index, std::array<unsigned int, 3>& dim) {
  unsigned int cellsPerLayer = dim[0] * dim[1];
  unsigned int layerCount = index / cellsPerLayer;
  index -= layerCount * cellsPerLayer;

  unsigned int lineCount = index / dim[0];
  unsigned int cellCount = index - lineCount * dim[0];

  return {cellCount, lineCount, layerCount};
}
std::array<unsigned int, 3> LinkedCellsContainer::getCoordFromVectorIndex(
    size_t index) {
  return LinkedCellsContainer::getCoordFromVectorIndexStatic(index, dimensions);
}

size_t LinkedCellsContainer::getCellIndexOfPosition(
    std::array<double, 3> &position) {
  std::array<double, 3> positionInBox = position - leftLowerCorner;
  std::array<int, 3> indexInBox{};
  for (int i = 0; i < 3; ++i) {
    indexInBox[i] = static_cast<int>(floor(positionInBox[i] / gridSize)) + 1;
    if (indexInBox[i] < 0) {
      indexInBox[i] = 0;
    }
    if (indexInBox[i] >= static_cast<int>(dimensions[i])) {
      indexInBox[i] = static_cast<int>(dimensions[i]) - 1;
    }
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
    std::function<void(Particle &, Particle &)> &binaryFunction) {
  recalculateStructure();

  for (size_t index = 0; index < cells.size(); ++index) {
    if (cells[index].type == cellType::halo) {
      continue;
    }
    for (size_t indexOffset : indexOffsetAdjacent) {
      if (indexOffset == 0) {
        auto &particles = cells[index].particles;
        for (auto first = particles.begin(); first != particles.end();
             ++first) {
          for (auto second = first; second != particles.end(); ++second) {
            if (*second == *first) continue;
            binaryFunction(particlesVector[*first], particlesVector[*second]);
          }
        }

      } else {
        for (auto cellAParticle : cells[index].particles) {
          for (auto cellBParticle : cells[index + indexOffset].particles) {
            binaryFunction(particlesVector[cellAParticle],
                           particlesVector[cellBParticle]);
          }
        }
      }
    }
  }
}

void LinkedCellsContainer::reserve(size_t amount) {
  particlesVector.reserve(amount);
}
size_t LinkedCellsContainer::capacity() { return particlesVector.capacity(); }
size_t LinkedCellsContainer::size() { return particlesVector.size(); }
void LinkedCellsContainer::emplace_back(std::array<double, 3> x_arg,
                                        std::array<double, 3> v_arg,
                                        double m_arg, int type) {
  particlesVector.emplace_back(x_arg, v_arg, m_arg, type);
  size_t index = getCellIndexOfPosition(x_arg);
  cells[index].particles.push_back(particlesVector.size() - 1);
}
void LinkedCellsContainer::recalculateStructure() {
  for (size_t cellIndex = 0; cellIndex < cells.size(); ++cellIndex) {
    auto &particles = cells[cellIndex].particles;
    for (auto it = particles.begin(); it != particles.end();) {
      auto pos = particlesVector[(*it)].getX();
      size_t correctIndex = getCellIndexOfPosition(pos);
      if (correctIndex != cellIndex) {
        cells[correctIndex].particles.push_back(*it);
        it = particles.erase(it);
      } else {
        ++it;
      }
    }
  }
}
