#include "LinkedCellsBoundary.h"
#include <cmath>
#include <vector>

LinkedCellsBoundary::LinkedCellsBoundary(cubeSide side, LinkedCellsContainer& container)
    : container{container} {

  // Add boundary cells
  for (auto cell : container.boundaryCells) {
    auto position = container.getCoordFromVectorIndex(cell->cellVectorIndex);

    auto dimIndex = getDimensionBySide(side);
    auto location = getCellGridLocation(side);

    bool isOnBoundarySide = position[dimIndex] == location;
    if (!isOnBoundarySide) continue;

    connectedCells.push_back(cell);
  }

  // Sort for better cache usage
  std::sort(connectedCells.begin(), connectedCells.end());
}

double LinkedCellsBoundary::getDistanceToWall(Particle const& particle) {
  // Assumes particle is linked with the cell it's currently in!
  size_t dimIndex = getDimensionBySide(side);

  double relativePosition =
      particle.getX()[dimIndex] - container.leftLowerCorner[dimIndex];

  return std::fmod(relativePosition, container.gridSize);
}

void LinkedCellsBoundary::deleteOutFlow() {
  for (auto cell : connectedCells) {
    for (auto neighborIndex : cell->neighborHaloIndices) {
      container.cells[neighborIndex].particles.clear();
    }
  }
}

using namespace ReflectiveBoundary;
void LinkedCellsBoundary::reflectParticles() {
  for (auto cell : connectedCells) {
    for (auto particle_index : cell->particles) {
      auto& particle = container.particlesVector[particle_index];
      auto distance = getDistanceToWall(particle);
      if (distance > reflectDistance || distance < -reflectDistance) continue;

      auto dimIndex = getDimensionBySide(side);

      auto ghostPos = particle.getX();
      ghostPos[dimIndex] += 2 * distance;

      Particle ghost{ghostPos};
      lennardJones.addForces(particle, ghost);
    }
  }
}

// CURRENTLY NOT USED, WILL PROBABLY BE DELETED BEFORE MASTER MERGE:
/*
namespace boundaryStrategy {
struct outflow {
}
};

struct reflective {
  static void behave() {}
};

struct periodic {};
}  // namespace boundaryStrategy
 */

/*
template <typename BoundaryStrategy>
auto applyBehavior() {
  for (auto& cell : connectedCells) {
    for (auto particle : (*cell)) {
      if (!isInRange(particle)) continue;

BoundaryStrategy::behave();
}
}
}
*/