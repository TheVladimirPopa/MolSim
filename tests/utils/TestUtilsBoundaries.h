#pragma once
#include "dataStructures/LinkedCellsContainer.h"
using v3d = std::array<double, 3>;

namespace TestUtils {
std::vector<Particle> applyBoundariesReturnParticles(LinkedCellsContainer& container, std::vector<v3d> positions) {
  for (auto& x : positions) container.emplace_back(x, v3d{1., 2., 3.}, 1.0, 1);

  LennardJonesModel model{3.0};
  size_t numberOfUses{0}; // useful for debugging
  std::function addForces{[&model, &numberOfUses](Particle& p1, Particle& p2) {
    numberOfUses++;
    model.addForces(std::forward<Particle&>(p1), std::forward<Particle&>(p2));
  }};

  container.forEachPair(addForces);

  std::vector<Particle> transformedParticles;
  std::function getParticles{[&transformedParticles](Particle& p) { transformedParticles.push_back(p); }};
  container.forEach(getParticles);

  return transformedParticles;
}

LinkedCellsContainer getBoundaryTestContainer(double halfSize, boundaryType type) {
  v3d leftCorner{-halfSize, -halfSize, -halfSize};
  v3d rightCorner{halfSize, halfSize, halfSize};

  LinkedCellsContainer container{2 * halfSize, leftCorner, rightCorner};
  container.setBoundaries({{cubeSide::LEFT, type},
                           {cubeSide::RIGHT, type},
                           {cubeSide::TOP, type},
                           {cubeSide::BOTTOM, type},
                           {cubeSide::FRONT, type},
                           {cubeSide::BACK, type}});

  return container;
}

std::vector<v3d> getTestPositions(double centerOffset) {
  // We check 6 surfaces, 12 edges, 8 corners. Any other case should occur
  // as a linear combination of these cases.
  return {// Particles in center of surface get reflected towards
          // the center coordinate (0,0,0)
          {v3d{centerOffset, 0.0, 0.0}},
          {v3d{-centerOffset, 0.0, 0.0}},
          {v3d{0.0, centerOffset, 0.0}},
          {v3d{0.0, -centerOffset, 0.0}},
          {v3d{0.0, 0.0, centerOffset}},
          {v3d{0.0, 0.0, -centerOffset}},

          // Particles in edges should be reflected to the center
          {v3d{centerOffset, centerOffset, 0}},
          {v3d{-centerOffset, centerOffset, 0}},
          {v3d{centerOffset, -centerOffset, 0}},
          {v3d{-centerOffset, -centerOffset, 0}},

          {v3d{0, centerOffset, centerOffset}},
          {v3d{0, -centerOffset, centerOffset}},
          {v3d{0, centerOffset, -centerOffset}},
          {v3d{0, -centerOffset, -centerOffset}},

          {v3d{centerOffset, 0, centerOffset}},
          {v3d{-centerOffset, 0, centerOffset}},
          {v3d{centerOffset, 0, -centerOffset}},
          {v3d{-centerOffset, 0, -centerOffset}},

          // Particle in corners should also be reflected to the center
          {v3d{centerOffset, centerOffset, centerOffset}},
          {v3d{-centerOffset, -centerOffset, -centerOffset}},
          {v3d{centerOffset, centerOffset, -centerOffset}},
          {v3d{centerOffset, -centerOffset, centerOffset}},
          {v3d{-centerOffset, centerOffset, centerOffset}},
          {v3d{-centerOffset, centerOffset, -centerOffset}},
          {v3d{-centerOffset, -centerOffset, centerOffset}},
          {v3d{centerOffset, -centerOffset, -centerOffset}}};
}
}