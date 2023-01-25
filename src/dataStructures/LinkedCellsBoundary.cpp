#include "LinkedCellsBoundary.h"

#include <cmath>
#include <vector>

#include "spdlog/spdlog.h"

LinkedCellsBoundary::LinkedCellsBoundary(CubeSide side, BoundaryType type, std::vector<cell>& cells,
                                         std::vector<Particle>* particlesVector, std::array<unsigned int, 3> dimensions,
                                         std::array<double, 3>* leftLowerCorner,
                                         std::array<double, 3>* rightUpperCorner)
    : side{side},
      type{type},
      cubeDimensions{dimensions},
      particlesVector{particlesVector},
      leftLowerCorner{leftLowerCorner},
      rightUpperCorner{rightUpperCorner} {
  // Add boundary cells
  for (cell& c : cells) {
    if (c.type != CellType::boundary) continue;
    auto position = c.getCellGridLocation();

    auto dimIndex = getDimensionBySide(side);
    auto location = getCellGridLocation(side);

    bool isOnBoundarySide = position[dimIndex] == location;
    if (!isOnBoundarySide) continue;

    connectedCells.push_back(&c);
  }

  // Add halo cells
  for (cell& c : cells) {
    if (c.type != CellType::halo) continue;

    auto position = c.getCellGridLocation();
    auto dimIndex = getDimensionBySide(side);

    // We add both the outermost layer of halos and the layer 1 coordinate
    // further in. The reason being: otherwise the grid gets holes at its
    // corners and deleteOutflow will not delete Particles in corner Halos.
    auto location = getCellGridLocation(side);      // For outflow boundary
    auto locationHalo = getHaloGridLocation(side);  // Any other boundary type

    bool isOnBoundarySide =
        (position[dimIndex] == location && type == BoundaryType::OUTFLOW) || position[dimIndex] == locationHalo;

    if (!isOnBoundarySide) continue;

    connectedHalos.push_back(&c);
  }

  // Sort for better cache availability
  std::sort(connectedCells.begin(), connectedCells.end());
}

double LinkedCellsBoundary::getDistanceToBoundary(Particle const& particle) const {
  // Assumes particle is linked with the cell it's currently in!
  size_t dimIndex = getDimensionBySide(side);

  bool leftRelative = side == CubeSide::LEFT || side == CubeSide::TOP || side == CubeSide::FRONT;
  double relativePosition = leftRelative ? particle.getX()[dimIndex] - (*leftLowerCorner)[dimIndex]
                                         : particle.getX()[dimIndex] - (*rightUpperCorner)[dimIndex];

  // can be negative, see header file.
  return relativePosition;
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
      auto distance = getDistanceToBoundary(particle);

      double reflectDistance = SIXTH_ROOT_OF_2 * particle.getSigma();

      if (distance > reflectDistance || distance < -reflectDistance) continue;

      auto dimIndex = getDimensionBySide(side);

      auto ghostPos = particle.getX();
      ghostPos[dimIndex] -= 2 * distance;

      Particle ghost{ghostPos};
      lennardJones.addForces(particle, ghost);
    }
  }
}

std::array<double, 3> LinkedCellsBoundary::getPeriodicLocation(Particle& particle) {
  size_t dimIndex = getDimensionBySide(side);

  // Choose either left or right corner as anker point
  bool leftRelative = side == CubeSide::LEFT || side == CubeSide::TOP || side == CubeSide::FRONT;
  double newLocation = leftRelative ? (*rightUpperCorner)[dimIndex] : (*leftLowerCorner)[dimIndex];
  newLocation += getDistanceToBoundary(particle);

  auto newPos = particle.getX();
  newPos[dimIndex] = newLocation;

  return newPos;
}

void LinkedCellsBoundary::teleportParticles() {
  for (cell* halo : connectedHalos) {
    if (halo->isEmpty()) continue;

    for (auto particleIndex : halo->particles) {
      Particle& particle = (*particlesVector)[particleIndex];

      if (!particle.isDeleted()) {
        std::array<double, 3> poz= particle.getX();
        particle.setX(getPeriodicLocation(particle));
        size_t dimIndex = getDimensionBySide(side);
        bool leftRelative = side == CubeSide::LEFT || side == CubeSide::TOP || side == CubeSide::FRONT;
        if (leftRelative) {
          particle.getPeriodicBoundariesCrossed()[dimIndex]--;
        } else {
          particle.getPeriodicBoundariesCrossed()[dimIndex]++;
        }
      }
    }
  }
}

void LinkedCellsBoundary::apply() {
  if (type == BoundaryType::OUTFLOW)
    deleteOutFlow();
  else if (type == BoundaryType::REFLECT) {
    reflectParticles();
  } else {
    teleportParticles();
  }
}
