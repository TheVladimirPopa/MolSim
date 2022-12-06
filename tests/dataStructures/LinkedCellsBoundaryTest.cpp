#include <cmath>

#include "dataStructures/LinkedCellsBoundary.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"

using v3d = std::array<double, 3>;

/**
 * Ensure internal structure of boundary is consistent with container.
 */
TEST(LinkedCellsBoundary, SetupDim2D) {
  double const meshWidth = 2.0;

  for (double i = 0.0; i < 10.0; i++) {
    for (double j = 0.0; j < 10.0; j++) {
      v3d dimensions{5 + i, 4 + j, 1};

      v3d leftCorner{0., 0., 0.};
      v3d rightCorner{meshWidth * dimensions};
      LinkedCellsContainer container{meshWidth, leftCorner, rightCorner};

      std::vector<LinkedCellsBoundary> boundaries{};
      LinkedCellsBoundary::setBoundaries(
          {{cubeSide::LEFT, boundaryType::OUTFLOW},
           {cubeSide::RIGHT, boundaryType::OUTFLOW},
           {cubeSide::TOP, boundaryType::REFLECT},
           {cubeSide::BOTTOM, boundaryType::REFLECT}},
          boundaries, container);

      // Number of generated boundaries is correct
      EXPECT_EQ(boundaries.size(), 4);

      // Each boundary is linked with the correct number of cells
      auto widthCellCount = container.getDimensions()[0] - 2;
      auto heightCellCount = container.getDimensions()[1] - 2;

      EXPECT_EQ(boundaries[0].getConnectedCells().size(), heightCellCount);
      EXPECT_EQ(boundaries[1].getConnectedCells().size(), heightCellCount);
      EXPECT_EQ(boundaries[2].getConnectedCells().size(), widthCellCount);
      EXPECT_EQ(boundaries[3].getConnectedCells().size(), widthCellCount);

      // Each boundary cell is only linked with halo cells als neighbours
      for (auto& boundary : boundaries) {
        for (auto& connectedCell : boundary.connectedCells) {
          for (auto neighbourIndex : connectedCell->neighborHaloIndices) {
            ASSERT_LT(neighbourIndex, container.getCellsVector().size())
                << "Neighbour index is outside vector.";
            ASSERT_GE(neighbourIndex, 0)
                << "Neighbour index is outside vector.";

            EXPECT_EQ(container.getCellsVector()[neighbourIndex].type,
                      cellType::halo);
          }
        }
      }
    }
  }
}

/**
 * TODO doku
 */
TEST(LinkedCellsBoundary, Outflow2D) {
  double const meshWidth = 2.0;

  v3d dimensions{5, 4, 1};
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{meshWidth * dimensions};

  LinkedCellsContainer container{meshWidth, leftCorner, rightCorner};

  std::vector<LinkedCellsBoundary> boundaries{};
  LinkedCellsBoundary::setBoundaries(
      {{cubeSide::LEFT, boundaryType::OUTFLOW},
       {cubeSide::RIGHT, boundaryType::OUTFLOW},
       {cubeSide::TOP, boundaryType::OUTFLOW},
       {cubeSide::BOTTOM, boundaryType::OUTFLOW}},
      boundaries, container);

  // Uniformly distributed particles;
  for (double x = leftCorner[0] - 2; x <= rightCorner[0] + 2; x += 0.4) {
    for (double y = leftCorner[1] - 2; y <= rightCorner[1] + 2; y += 0.4) {
      container.emplace_back(v3d{x, y, 0.5}, v3d{0, 0, 0}, 1, 0);
    }
  }

  container.recalculateStructure();

  // Helper to count particles in container and in halo
  auto countParticles = [&container] {
    size_t countedSize{};
    size_t countedHaloSize{};
    for (auto& cell : container.getCellsVector()) {
      if (cell.type == cellType::halo) countedHaloSize += cell.particles.size();

      countedSize += cell.particles.size();
    }

    return std::pair{countedSize, countedHaloSize};
  };

  // Helper that utilized .foreach()
  size_t foreachCount{};
  std::function count{[&foreachCount](Particle& p) { ++foreachCount; }};

  // Count particles using the helper
  size_t containerSizeBefore{container.size()};
  auto [countedSize, countedHaloSize] = countParticles();

  container.forEach(count);
  size_t forEachCountBefore = foreachCount;
  foreachCount = 0;

  EXPECT_EQ(containerSizeBefore, countedSize)
      << "It's possible that some particles are not linked or linked multiple "
         "times.";

  // Apply boundaries, which should delete the halo
  for (auto& boundary : boundaries) boundary.apply();
  EXPECT_EQ(countParticles().second, 0);

  // Ensure that a restructure doesn't let particles reappear
  container.recalculateStructure();
  EXPECT_EQ(countParticles().second, 0);

  // Check if particles actually are now deleted
  auto [countedSizeAfter, countedHaloSizeAfter] = countParticles();
  auto numberOfDeletions = countedHaloSize - countedHaloSizeAfter;

  EXPECT_GT(numberOfDeletions, 0);
  EXPECT_EQ(countedSizeAfter, countedSize - numberOfDeletions);
  EXPECT_EQ(countedHaloSizeAfter, 0);
  EXPECT_EQ(container.size() + numberOfDeletions, containerSizeBefore);

  // Check if .foreach() works correctly
  container.forEach(count);
  size_t forEachCountAfter = foreachCount;
  EXPECT_GT(forEachCountBefore, forEachCountAfter)
      << "Foreach must not apply to deleted particles.";
}

TEST(LinkedCellsBoundary, Reflective) {
  v3d leftCorner{-5.0, -5.0, -5.0};
  v3d rightCorner{5.0, 5.0, 5.0};

  LinkedCellsContainer container{10.0, leftCorner, rightCorner};

  std::vector<LinkedCellsBoundary> boundaries{};
  LinkedCellsBoundary::setBoundaries(
      {{cubeSide::LEFT, boundaryType::REFLECT},
       {cubeSide::RIGHT, boundaryType::REFLECT},
       {cubeSide::TOP, boundaryType::REFLECT},
       {cubeSide::BOTTOM, boundaryType::REFLECT},
       {cubeSide::FRONT, boundaryType::REFLECT},
       {cubeSide::BACK, boundaryType::REFLECT}},
      boundaries, container);

  double epsilon = ReflectiveBoundary::reflectDistance / 5.0;
  double centerOffset = 5.0 - epsilon;

  // We check 6 surfaces, 12 edges, 8 corners. Any other case should occur
  // as a linear combination of these cases.
  std::vector<v3d> testParticles = {
      // Particles in center of surface get reflected towards
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
      {v3d{centerOffset, -centerOffset, -centerOffset}}

  };

  for (auto particleX : testParticles)
    container.emplace_back(particleX, {0.0, 0.0, 0.0}, 1.0, 0.0);

  for (auto& boundary : boundaries) boundary.apply();

  std::vector<Particle> transformedParticles{};
  std::function copyParticles{[&transformedParticles](Particle& p) {
    transformedParticles.push_back(p);
  }};
  container.forEach(copyParticles);

  ASSERT_EQ(testParticles.size(), transformedParticles.size())
      << "Particle count unexpectedly changed when applying reflective "
         "boundary.";

  // The force that now applies to each of the particles now should face towards
  // the center coordinate. Thanks to the test setup is suffices to just check
  // whether forces and location share opposite signs.

  // Todo: ARRAYUTILS TEMPLATE
  auto signum = [](double x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
  };

  for (auto& particle : transformedParticles) {
    // Figure out signs of forces + location per dimension
    v3d positionSigns = particle.getX();
    v3d invForceSigns = (-1.0) * particle.getF();

    for (int i = 0; i < 3; i++) {
      positionSigns[i] = signum(positionSigns[i]);
      invForceSigns[i] = signum(invForceSigns[i]);
    }

    EXPECT_GT(ArrayUtils::L2Norm(invForceSigns), 0)
        << "No forces where applied onto particle.";

    if (ArrayUtils::L2Norm(invForceSigns)) {
      EXPECT_EQ(positionSigns, invForceSigns)
          << "Particle is not symmetrically reflected from boundaries.";
    }
  }

  // Do not influence particles that are out of reach
  // Apply force onto particles very close to the wall
  // Ensure force direction is correct for all planes (namely away from plane)
}
