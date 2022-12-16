#pragma once
#include "dataStructures/LinkedCellsContainer.h"
using v3d = std::array<double, 3>;

namespace TestUtils {
/**
 * Maps a double onto its sign.
 * @param x The double that gets translated into -1, 0 or 1.
 * @return The sign represented as -1, 0 or 1.
 */
auto signum(double x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
};

/**
 * Maps a double vector of size 3 into its signs
 * @param x Double vector of size 3
 * @return Vector of size 3 with the respective sign as its components
 */
auto signum(v3d x) {
  for (int i = 0; i < 3; i++) {
    if (x[i] > 0)
      x[i] = 1.0;
    else if (x[i] < 0)
      x[i] = -1.0;
    else
      x[i] = 0.0;
  }

  return x;
};

/**
 * Takes a LinkedCells container and multiple positions, generates particles on these positions, executes a Lennard
 * Jones force calculation and returns the transformed particles.
 * @param container LinkedCells container that gets populated with particles
 * @param positions Positions of the particles
 * @return Transformed particles with Lennard Jones forces
 */
std::vector<Particle> applyBoundariesReturnParticles(LinkedCellsContainer& container, std::vector<v3d> positions) {
  for (auto& x : positions) container.emplace_back(x, v3d{1., 2., 3.}, 1.0, 1);

  double const cutOff = 3.0;  // tests will break if this is changed, as forces start to appear/disappear accordingly
  LennardJonesModel model{cutOff};

  size_t numberOfUses{0};  // useful for debugging
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

/**
 * Creates a LinkedCells container that is center at (0, 0, 0).
 * @param halfSize half of the lengths of an edge of the container. (halfSize,0,0) is the center of the right plane.
 * @param gridSize Size of the individual cells of the container
 * @param type Type of boundary that gets generated. E. g. PERIODIC or REFLECTIVE
 * @return The created LinkedCells container
 */
LinkedCellsContainer getBoundaryTestContainer(double halfSize, double gridSize, BoundaryType type) {
  v3d leftCorner{-halfSize, -halfSize, -halfSize};
  v3d rightCorner{halfSize, halfSize, halfSize};

  LinkedCellsContainer container{gridSize, leftCorner, rightCorner};
  container.setBoundaries({{CubeSide::LEFT, type},
                           {CubeSide::RIGHT, type},
                           {CubeSide::TOP, type},
                           {CubeSide::BOTTOM, type},
                           {CubeSide::FRONT, type},
                           {CubeSide::BACK, type}});

  return container;
}

/**
 * Analogously to getBoundaryTestContainer(double, double, boundaryType)
 * @param halfSize Analogously. gridSize = 2 * halfSize
 * @param type Analogously
 * @return Analogously
 */
LinkedCellsContainer getBoundaryTestContainer(double halfSize, BoundaryType type) {
  return getBoundaryTestContainer(halfSize, 2 * halfSize, type);
}

/**
 * Generates locations that form a cube. 26 in total. (8 corner, 6 in the center of the surfaces, 12 along the edges.)
 * @param centerOffset The offset per dimension to 0
 * @return The generated locations
 */
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
}  // namespace TestUtils