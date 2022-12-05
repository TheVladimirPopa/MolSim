#include "LinkedCellsBoundary.h"

LinkedCellsBoundary::LinkedCellsBoundary(cubeSide side, LinkedCellsContainer& container)
    : container{container} {

  // Add boundary cells
  for (auto cell : container.boundaryCells) {
    auto position = container.getCoordFromVectorIndex(cell->cellVectorIndex);

    auto dimIndex = dimLocation.at(side).first;
    auto location = dimLocation.at(side).second;

    bool isOnBoundarySide = position[dimIndex] == location;
    if (!isOnBoundarySide) continue;

    connectedCells.push_back(cell);
  }

  // Sort for better cache usage
  std::ranges::sort(connectedCells);
}

double LinkedCellsBoundary::getDistanceToWall(Particle const& particle) {
  // Assumes particle is linked with the cell it's currently in!
  auto dimIndex = dimLocation.at(side).first; // todo: das ist zu langsam

  double relativePosition =
      particle.getX()[dimIndex] - container.leftLowerCorner[dimIndex];

  return std::fmod(relativePosition, container.gridSize);
}

void LinkedCellsBoundary::deleteOutFlow() {
  for (auto cell : connectedCells) {
    // todo: in interface der zelle
    for (auto neighborIndex : cell->neighborHaloIndices) {
      container.cells[neighborIndex].particles.clear();
    }
  }
}

void LinkedCellsBoundary::reflectParticles() {
  for (auto cell : connectedCells) {
    for (auto particle_index : cell->particles) {
      auto& particle = container.particlesVector[particle_index];
      auto distance = getDistanceToWall(particle);
      if (distance > viewDistance || distance < -viewDistance) continue;

      auto dimIndex = dimLocation.at(side).first; // todo: das ist zu langsam

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
  static void behave() {
    // Todo: There are 2 ways to go about this:
    //  delete everything in cutoff distance
    //  Or: Wait till particle passed boundary. Then delete manually
    //  The second case avoids bugs  caused by a too big delta_t
    //  But it's a bit more expansive, as we have to relink
    // Both reflective and outflow hat to

// Also wir brauchen ein Verhalten, was für die inneren Blöcke gemacht wird
// und was jeweils per gelinktem auesseren Block gemacht wird.
}
};

struct reflective {
  // Todo:
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