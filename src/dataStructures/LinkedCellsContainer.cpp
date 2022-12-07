//
// Created by leo on 26.11.22.
//

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
    dimensions[i] =
        static_cast<size_t>(floor(boundingBoxDim[i] / gridSize)) + 2;
    // Make sure each dimension has at least one boundary cell
    if (dimensions[i] == 2) {
      dimensions[i] = 3;
    }
    numCells *= dimensions[i];
  }

  cells.reserve(numCells);

  // Instantiate all the cells with their specified type
  for (size_t z = 0; z < dimensions[2]; ++z) {
    for (size_t y = 0; y < dimensions[1]; ++y) {
      for (size_t x = 0; x < dimensions[0]; ++x) {
        if (x == 0 or y == 0 or z == 0 or x == dimensions[0] - 1 or
            y == dimensions[1] - 1 or z == dimensions[2] - 1) {
          cells.emplace_back(halo);
        } else if (x == 1 or y == 1 or z == 1 or x == dimensions[0] - 2 or
                   y == dimensions[1] - 2 or z == dimensions[2] - 2) {
          cells.emplace_back(boundary);

        } else {
          cells.emplace_back(inner);
        }
      }
    }
  }

  // Precompute the indexOffsets, which are used to quickly determine the
  // adjacent cells
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
size_t LinkedCellsContainer::getCellIndexOfPosition(
    std::array<double, 3> &position) {
  std::array<double, 3> positionInBox = position - leftLowerCorner;
  std::array<int, 3> indexInBox{};

  for (int i = 0; i < 3; ++i) {
    //+1 has to be added because of the 0 cell in each dimension is a halo cell
    indexInBox[i] = static_cast<int>(floor(positionInBox[i] / gridSize)) + 1;

    // If the position is far outside the domain bounding box  map, set to
    // according halo box
    if (indexInBox[i] < 0) {
      indexInBox[i] = 0;
    }
    // If the particle is currently placed in a halo cell, it has to be tested
    // if it is still in the domain box, since the cell size of the last cell
    // may be bigger
    if (indexInBox[i] >= static_cast<int>(dimensions[i]) - 2) {
      if (position[i] > rightUpperCorner[i]) {
        // Place it in the halo cell
        indexInBox[i] = static_cast<int>(dimensions[i]) - 1;
      } else {
        // Place it in the boundary cell
        indexInBox[i] = static_cast<int>(dimensions[i]) - 2;
      }
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
    // Don't do anything for particles a halo cell
    if (cells[index].type == halo) {
      continue;
    }
    for (size_t indexOffset : indexOffsetAdjacent) {
      // Special case to match particles within one cell
      if (indexOffset == 0) {
        auto &particles = cells[index].particles;
        for (auto first = particles.begin(); first != particles.end();
             ++first) {
          for (auto second = first; second != particles.end(); ++second) {
            // Don't run the function on a pair consisting of the same particle
            if (*second == *first) continue;
            binaryFunction(particlesVector[*first], particlesVector[*second]);
          }
        }

      } else {
        // Loop so the particles of each of the two cells and match them
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
  // Loop through each cell
  for (size_t cellIndex = 0; cellIndex < cells.size(); ++cellIndex) {
    // Loop through each particle of that cell
    auto &particles = cells[cellIndex].particles;
    for (auto it = particles.begin(); it != particles.end();) {
      auto pos = particlesVector[(*it)].getX();
      size_t correctIndex = getCellIndexOfPosition(pos);
      if (correctIndex != cellIndex) {
        // Delete particle and push it into correct cell, if it is currently in
        // the wrong cell
        cells[correctIndex].particles.push_back(*it);
        it = particles.erase(it);
      } else {
        ++it;
      }
    }
  }
}
