#pragma once
#include "dataStructures/LinkedCellsContainer.h"

class LinkedCellsContainerColouringMultiple : public LinkedCellsContainer {
#ifdef _OPENMP
 private:
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

  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) override {
    recalculateStructure();
    applyBoundaries();

    if (hasPeriodicBoundaries) {
      // TODO: SAVE THIS CALL BY MANUALLY RELINKING PARTICLES!
      recalculateStructure();
      applyPeriodicForces(binaryFunction);
    }

    for (auto &colour : colourList) {
#pragma omp parallel for default(none) shared(binaryFunction, cells, colour) schedule(dynamic)
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
                  binaryFunction(particlesVector[cellAParticle], particlesVector[cellBParticle]);
                }
              }
            }
          }
        }
      }
    }
  }

  [[nodiscard]] const std::vector<std::vector<std::array<size_t, 4>>> &getColourList() const { return colourList; }
#endif
};
