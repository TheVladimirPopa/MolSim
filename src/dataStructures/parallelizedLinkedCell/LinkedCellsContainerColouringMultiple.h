#pragma once
#include "dataStructures/LinkedCellsContainer.h"

/**
 * Uses domain colouring to ensure that no race conditions can occur
 * Each colour is a vector of an array of cell indices, which are arranged that no neighbouring cell of such an array
 * block is overlapping with an other array block. To achieve this 8 colours were used. One thread therefore always has
 * to work in a block of 4 cells.
 *  \image html colouringMultiple.gif width=900px
 *  \image latex colouringMultiple.gif "Colouring schema" width = [goodwidth]cm
 */
class LinkedCellsContainerColouringMultiple : public LinkedCellsContainer {
 private:
  /**
   * Is a vector of vectors of 4 element arrays of indices into the cell vector.
   * @note These are organized in a way that all cells within a colour the inner VECOTR can be processed in parallel
   * without any race conditions, when each thread is assigned a complete array of indices
   */
  std::vector<std::vector<std::array<size_t, 4>>> colourList{};

 public:
  LinkedCellsContainerColouringMultiple(double cellSize, std::array<double, 3> &leftLowerBound,
                                        std::array<double, 3> &rightUpperBound)
      : LinkedCellsContainer(cellSize, leftLowerBound, rightUpperBound) {
    for (int i = 0; i < 8; ++i) {
      colourList.emplace_back();
    }

    for (size_t z = 1; z < dimensions[2] - 1; ++z) {
      for (size_t y = 1; y < dimensions[1] - 1; y += 2) {
        for (size_t x = 1; x < dimensions[0] - 1; x += 2) {
          size_t index = (x / 2) % 2;
          index += ((y / 2) % 2) * 2;
          if (z % 2 == 0) {
            index += 4;
          }
          colourList[index].emplace_back(
              std::array<size_t, 4>{getVectorIndexFromCoord(x, y, z), getVectorIndexFromCoord(x + 1, y, z),
                                    getVectorIndexFromCoord(x, y + 1, z), getVectorIndexFromCoord(x + 1, y + 1, z)});
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
      for (auto &colourGroup : colour) {
        for (auto index : colourGroup) {
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
    }

    for (auto &structure : structuresVector) {
      if (structure.hasArtificalForces()) structure.applyArtificialForces();
    }
  }

  [[nodiscard]] const std::vector<std::vector<std::array<size_t, 4>>> &getColourList() const { return colourList; }
#endif
};
