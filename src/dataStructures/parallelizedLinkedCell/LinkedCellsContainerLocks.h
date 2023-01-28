#pragma once
#include "dataStructures/LinkedCellsContainer.h"

class LinkedCellsContainerLocks : public LinkedCellsContainer {
 public:
  LinkedCellsContainerLocks(double cellSize, std::array<double, 3> &leftLowerBound,
                            std::array<double, 3> &rightUpperBound)
      : LinkedCellsContainer(cellSize, leftLowerBound, rightUpperBound) {}
#ifdef _OPENMP

 public:
  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) override {
    recalculateStructure();
    applyBoundaries();

    if (hasPeriodicBoundaries) {
      recalculateStructure();
      applyPeriodicForces(binaryFunction);
    }

#pragma omp parallel for default(none) shared(binaryFunction, cells) schedule(static, 8)
    for (size_t index = 0; index < cells.size(); ++index) {
      if (cells[index].type == CellType::halo || cells[index].isEmpty()) {
        continue;
      }
      cells[index].lock();
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
          cells[index + indexOffset].lock();
          // Loop so the particles of each of the two cells and match them
          for (auto indexA : cells[index].particles) {
            for (auto indexB : cells[index + indexOffset].particles) {
              binaryFunction(particlesVector[indexA], particlesVector[indexB]);
            }
          }
          cells[index + indexOffset].unlock();
        }
      }
      cells[index].unlock();
    }

    for (auto &structure : structuresVector) {
      if (structure.hasArtificalForces()) structure.applyArtificialForces();
    }
  }
#endif
};