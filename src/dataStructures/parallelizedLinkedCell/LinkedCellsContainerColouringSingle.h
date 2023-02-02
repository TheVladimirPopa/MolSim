#pragma once
#include "dataStructures/LinkedCellsContainer.h"

/**
 * Uses domain colouring to ensure that no race conditions can occur
 * Each colour is a vector of cell indices, which are arranged that no neighbouring cell is overlapping
 * To achieve this 18 colours were used
 *  \image html colouringSingle.gif width=900px
 *  \image latex colouringSingle.gif "Colouring schema" width = [goodwidth]cm
 */
class LinkedCellsContainerColouringSingle : public LinkedCellsContainer {
 private:
  /**
   * Is a vector of vectors of indices into the cell vector.
   * @note These are organized in a way that all cells within a colour can be processed in parallel without any race
   * conditions
   */
  std::vector<std::vector<size_t>> colourList{};

 public:
  LinkedCellsContainerColouringSingle(double cellSize, std::array<double, 3> &leftLowerBound,
                                      std::array<double, 3> &rightUpperBound)
      : LinkedCellsContainer(cellSize, leftLowerBound, rightUpperBound) {
    for (int i = 0; i < 18; ++i) {
      colourList.emplace_back();
    }

    for (size_t z = 1; z < dimensions[2] - 1; ++z) {
      for (size_t y = 1; y < dimensions[1] - 1; ++y) {
        for (size_t x = 1; x < dimensions[0] - 1; ++x) {
          size_t index = x % 3;
          index += (y % 3) * 3;
          if (z % 2 == 0) {
            index += 9;
          }
          colourList[index].push_back(getVectorIndexFromCoord(x, y, z));
        }
      }
    }
  }

#ifdef _OPENMP
  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) override {
    recalculateStructure();
    applyBoundaries();

    if (hasPeriodicBoundaries) {
      recalculateStructure();
      applyPeriodicForces(binaryFunction);
    }

    for (auto &colour : colourList) {
#pragma omp parallel for default(none) shared(binaryFunction, cells, colour) schedule(guided)
      for (auto index : colour) {
        if (cells[index].type == CellType::halo || cells[index].isEmpty()) {
          continue;
        }
        for (size_t indexOffset : indexOffsetAdjacent) {
          // Special case to match particles within one cell
          if (indexOffset == 0) {
            auto &particles = cells[index].particles;
            for (auto first = particles.begin(); first != particles.end(); ++first) {
              for (auto second = std::next(first); second != particles.end(); ++second) {
                binaryFunction(particlesVector[*first], particlesVector[*second]);
              }
            }

          } else {
            // Loop so the particles of each of the two cells and match them
            for (auto indexA : cells[index].particles) {
              for (auto indexB : cells[index + indexOffset].particles) {
                binaryFunction(particlesVector[indexA], particlesVector[indexB]);
              }
            }
          }
        }
      }
    }

    for (auto &structure : structuresVector) {
      if (structure.hasArtificalForces()) structure.applyArtificialForces();
    }
  }

  // For testing purposes
  const std::vector<std::vector<size_t>> &getColourList() const { return colourList; }
#endif
};
