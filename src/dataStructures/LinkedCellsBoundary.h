#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include "LinkedCellsContainer.h"
#include "utils/ArrayUtils.h"
#include "model/LennardJonesModel.h"

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

// Imagine a cube on an XYZ grid, centered at (0,0,0). The enum describes the
// direction of a surface pointing vector with initial point (0,0,0).

enum class cubeSide {
  LEFT = 0,    // dimension[0]
  RIGHT = 1,   // dimension[0]
  TOP = 2,     // dimension[1]
  BOTTOM = 3,  // dimension[1]
  FRONT = 4,   // dimension[2]
  BACK = 5     // dimension[2]
};

// todo: do this properly
static constexpr double CUTOFFRADIUS{42}; // TODO: use correct value
static const LennardJonesModel lennardJones{CUTOFFRADIUS};
constexpr double SIGMA = 1.0;  // TODO: nicht hardcoden
constexpr double SIXTH_ROOT_OF_2 =
    1.1224620483093729814335330496791795162324111106139867534404095458;
const double viewDistance = SIXTH_ROOT_OF_2 * SIGMA;

class LinkedCellsBoundary {
 private:
  cubeSide side;
  LinkedCellsContainer& container;
  // std::array<int, 3>& cubeDimension;

  struct dimensionAndPosition {
    size_t dimensionIndex;
    unsigned int position;
  };

  std::unordered_map<cubeSide, std::pair<size_t, unsigned int>> const
      dimLocation = {
          // Map boundary side onto dimension and index in dimension
          {cubeSide::LEFT,   {0, 1}},
          {cubeSide::RIGHT,  {0, container.dimensions[0] - 2}},
          {cubeSide::TOP,    {1, 1}},
          {cubeSide::BOTTOM, {1, container.dimensions[1] - 2}},
          {cubeSide::FRONT,  {2, 1}},
          {cubeSide::BACK,   {2, container.dimensions[2] - 2}}
  };

 public:
  std::vector<LinkedCellsContainer::boundaryCell*> connectedCells;
  virtual void applyBoundaryLogic();

  // Boundaries are like sheets of papers with two different sides in 3D space.
  // One side influences close particles. The other side does nothing.
  LinkedCellsBoundary(cubeSide side, LinkedCellsContainer& container)
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

    // Sort pointers ascending to optimize cache availability
    // todo: check if this sorts pointer or values
    std::ranges::sort(connectedCells);
  }

  double getDistanceToWall(Particle const& particle) {
    // Assumes particle is linked with the cell it's currently in!
    auto dimIndex = dimLocation.at(side).first; // todo: das ist zu langsam

    double relativePosition =
        particle.getX()[dimIndex] - container.leftLowerCorner[dimIndex];

    return std::fmod(relativePosition, container.gridSize);
  }

  void deleteOutFlow() {
    for (auto cell : connectedCells) {
      // todo: in interface der zelle
      for (auto neighborIndex : cell->neighborHaloIndices) {
        container.cells[neighborIndex].particles.clear();
      }
    }
  }

  void reflectParticles() {
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
};
