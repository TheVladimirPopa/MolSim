#include "LinkedCellsBoundary.h"

#include <cmath>
#include <vector>

#include "spdlog/spdlog.h"

// todo:
#define PERIODIC_CUTOFF_PLACEHOLDER 3.0

LinkedCellsBoundary::LinkedCellsBoundary(cubeSide side, boundaryType type, std::vector<cell>& cells,
                                         std::vector<Particle>* particlesVector, std::vector<Particle>* ghostVector,
                                         std::array<unsigned int, 3> dimensions, std::array<double, 3> leftLowerCorner,
                                         std::array<double, 3> rightUpperCorner)
    : side{side},
      type{type},
      cubeDimensions{dimensions},
      particlesVector{particlesVector},
      ghostParticlesVector{ghostVector},
      leftLowerCorner{leftLowerCorner},
      rightUpperCorner{rightUpperCorner} {
  // Add boundary cells (for periodic boundary this includes halo cells)
  for (cell& c : cells) {
    if (type != boundaryType::PERIODIC && c.type != cellType::boundary) continue;
    auto position = getCoordFromVectorIndex(c.cellVectorIndex);

    auto dimIndex = getDimensionBySide(side);
    auto location = getCellGridLocation(side);

    bool isOnBoundarySide = position[dimIndex] == location;
    if (!isOnBoundarySide) continue;

    connectedCells.push_back(&c);
  }

  // Add halo cells
  for (cell& c : cells) {
    if (c.type != cellType::halo) continue;

    auto position = getCoordFromVectorIndex(c.cellVectorIndex);
    auto dimIndex = getDimensionBySide(side);

    // We add both the outermost layer of halos and the layer 1 coordinate
    // further in. The reason being: otherwise the grid gets holes at its
    // corners and deleteOutflow will not delete Particles in corner Halos.
    auto location = getCellGridLocation(side);
    auto locationHalo = getHaloGridLocation(side);

    bool isOnBoundarySide = position[dimIndex] == location || position[dimIndex] == locationHalo;

    if (!isOnBoundarySide) continue;

    connectedHalos.push_back(&c);
  }

  // Sort for better cache usage
  std::sort(connectedCells.begin(), connectedCells.end());
}

double LinkedCellsBoundary::getDistanceToWall(Particle const& particle) const {
  // Assumes particle is linked with the cell it's currently in!
  size_t dimIndex = getDimensionBySide(side);

  bool leftRelative = side == cubeSide::LEFT || side == cubeSide::TOP || side == cubeSide::FRONT;
  double relativePosition = leftRelative ? particle.getX()[dimIndex] - leftLowerCorner[dimIndex]
                                         : particle.getX()[dimIndex] - rightUpperCorner[dimIndex];

  // can be negative, see header file.
  return relativePosition;
}

std::array<unsigned int, 3> LinkedCellsBoundary::getCoordFromVectorIndex(size_t index) const {
  unsigned int cellsPerLayer = cubeDimensions[0] * cubeDimensions[1];
  unsigned int layerCount = index / cellsPerLayer;
  index -= layerCount * cellsPerLayer;

  unsigned int lineCount = index / cubeDimensions[0];
  unsigned int cellCount = index - (lineCount * cubeDimensions[0]);

  return {cellCount, lineCount, layerCount};
}

void LinkedCellsBoundary::deleteOutFlow() {
  for (auto cell : connectedHalos) {
    if (cell->isEmpty()) continue;

    for (auto particleIndex : cell->particles) {
      (*particlesVector)[particleIndex].deleteParticle();
      spdlog::debug("Deleted particle");
    }

    cell->particles.clear();
  }
}

using namespace ReflectiveBoundary;
void LinkedCellsBoundary::reflectParticles() {
  for (cell* cell : connectedCells) {
    for (auto particleIndex : cell->particles) {
      auto& particle = (*particlesVector)[particleIndex];
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

std::array<double, 3> LinkedCellsBoundary::getMirrorLocation(Particle& particle) {
  size_t dimIndex = getDimensionBySide(side);

  // leftRelative means the boundary location depends on the left corner of the counter
  bool leftRelative = side == cubeSide::LEFT || side == cubeSide::TOP || side == cubeSide::FRONT;

  // to find the new location we then must use the right corner and place the particle with the same offset
  double boundaryOffset = getDistanceToWall(particle);
  double newLocation =
      leftRelative ? rightUpperCorner[dimIndex] + boundaryOffset : leftLowerCorner[dimIndex] + boundaryOffset;

  // todo: inefficient for not good reason
  auto newPos = particle.getX();
  newPos[dimIndex] = newLocation;
  return newPos;
}

void LinkedCellsBoundary::addGhostForces(size_t index) {
  Particle& particle = (*particlesVector)[index];
  if (!particle.isDeleted() && getDistanceToWall(particle) > PERIODIC_CUTOFF_PLACEHOLDER) return;

  Particle ghost{particle};
  ghost.setX(getMirrorLocation(particle));
  ghostParticlesVector->push_back(std::move(ghost));
}

void LinkedCellsBoundary::loopSpace() {
  // Part 1: Simulate loop space by mirroring particles in boundary cells to influence particles on the other side
  for (cell* bound : connectedCells) {
    for (auto particleIndex : bound->particles) {
      addGhostForces(particleIndex);
    }
  }

  // Part 2: Teleport particles that pass through a boundary cell
  for (cell* halo : connectedHalos) {
    if (halo->isEmpty()) continue;

    for (auto particleIndex : halo->particles) {
      Particle& particle = (*particlesVector)[particleIndex];

      if (!particle.isDeleted()) particle.setX(getMirrorLocation(particle));
    }
  }
}

void LinkedCellsBoundary::apply() {
  if (type == boundaryType::OUTFLOW)
    deleteOutFlow();
  else if (type == boundaryType::REFLECT) {
    reflectParticles();
  } else {
    loopSpace();
  }
}
