#include "LinkedCellsBoundary.h"

#include <vector>

#include "spdlog/spdlog.h"

LinkedCellsBoundary::LinkedCellsBoundary(cubeSide side, boundaryType type,
                                         LinkedCellsContainer& container)
    : side{side}, type{type}, container{container} {
  // Add boundary cells
  for (auto& cell : container.boundaryCells) {
    auto position = container.getCoordFromVectorIndex(cell->cellVectorIndex);

    auto dimIndex = getDimensionBySide(side);
    auto location = getCellGridLocation(side);

    bool isOnBoundarySide = position[dimIndex] == location;
    if (!isOnBoundarySide) continue;

    connectedCells.push_back(cell);
  }

  // Add halo cells
  for (cell& c : container.cells) {
    if (c.type != cellType::halo) continue;

    auto position = container.getCoordFromVectorIndex(c.cellVectorIndex);
    auto dimIndex = getDimensionBySide(side);

    // We add both the outermost layer of halos and the layer 1 coordinate
    // further in. The reason being: otherwise the grid gets holes at its
    // corners and deleteOutflow will not delete Particles in corner Halos.
    auto location = getCellGridLocation(side);
    auto locationHalo = getHaloGridLocation(side);

    bool isOnBoundarySide =
        position[dimIndex] == location || position[dimIndex] == locationHalo;

    if (!isOnBoundarySide) continue;


    connectedHalos.push_back(&c);
  }


  // Sort for better cache usage
  std::sort(connectedCells.begin(), connectedCells.end());
}

double LinkedCellsBoundary::getDistanceToWall(Particle const& particle) {
  // Assumes particle is linked with the cell it's currently in!
  size_t dimIndex = getDimensionBySide(side);

  bool leftRelative = side == cubeSide::LEFT || side == cubeSide::TOP ||
                      side == cubeSide::FRONT;
  double relativePosition =
      leftRelative
          ? particle.getX()[dimIndex] - container.leftLowerCorner[dimIndex]
          : particle.getX()[dimIndex] - container.rightUpperCorner[dimIndex];

  return relativePosition;
}
/*
void LinkedCellsBoundary::linkOutflowNeighbors() {
  for (auto& cell : connectedCells) {
    std::vector<size_t> neighbors{};
    // Add halos reachable with 1 step
    for (size_t directNeighbor : cell->getDirectNeighbors(container.dimensions)) {
      if (container.getCellsVector()[directNeighbor].type != cellType::halo)
        continue;

      neighbors.push_back(directNeighbor);
    }



    // Add halos reachable
  }
}*/

void LinkedCellsBoundary::deleteOutFlow() {
  for (auto cell : connectedHalos) {
    if (cell->particles.empty()) continue;

    for (auto particleIndex : cell->particles) {
      container.particlesVector[particleIndex].deleteParticle();
      spdlog::debug("Deleted particle");
    }

    cell->particles.clear();
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
      ghostPos[dimIndex] -= 2 * distance;

      Particle ghost{ghostPos};
      lennardJones.addForces(particle, ghost);
    }
  }
}

void LinkedCellsBoundary::setBoundaries(
    std::vector<std::pair<cubeSide, boundaryType>> sideAndType,
    std::vector<LinkedCellsBoundary>& output, LinkedCellsContainer& container) {
  // Ensure that boundary[0] always is the LEFT one, etc.
  std::sort(sideAndType.begin(), sideAndType.end(),
            [](std::pair<cubeSide, boundaryType> lhs, auto rhs) {
              return static_cast<int>(lhs.first) < static_cast<int>(rhs.first);
            });

  for (auto [side, type] : sideAndType) {
    LinkedCellsBoundary boundary{side, type, container};
    output.push_back(boundary);
  }

  container.boundaries = &output;
}

void LinkedCellsBoundary::apply() {
  if (type == boundaryType::OUTFLOW)
    deleteOutFlow();
  else if (type == boundaryType::REFLECT) {
    reflectParticles();
  }
}
