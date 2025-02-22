#include <cmath>

#include "dataStructures/LinkedCellsBoundary.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"
#include "utils/TestUtilsBoundaries.h"
using TestUtils::signum;

using v3d = std::array<double, 3>;

/**
 * Ensure internal structure of boundary is consistent with container.
 */
TEST(LinkedCellsBoundary, SetupStructure) {
  double const meshWidth = 2.0;

  for (int i = 0.0; i < 10.0; i++) {
    for (int j = 0.0; j < 10.0; j++) {
      v3d dimensions{5 + static_cast<double>(i), 4 + static_cast<double>(j), 1};

      v3d leftCorner{0., 0., 0.};
      v3d rightCorner{meshWidth * dimensions};
      LinkedCellsContainer container{meshWidth, leftCorner, rightCorner};

      container.setBoundaries({{CubeSide::LEFT, BoundaryType::OUTFLOW},
                               {CubeSide::RIGHT, BoundaryType::OUTFLOW},
                               {CubeSide::TOP, BoundaryType::REFLECT},
                               {CubeSide::BOTTOM, BoundaryType::REFLECT}});

      // Number of generated boundaries is correct
      auto& boundaries = container.getBoundaries();
      EXPECT_EQ(boundaries.size(), 4);

      // Each boundary is linked with the correct number of cells
      auto widthCellCount = container.getDimensions()[0] - 2;
      auto heightCellCount = container.getDimensions()[1] - 2;

      EXPECT_EQ(boundaries[0].getConnectedCells().size(), heightCellCount);
      EXPECT_EQ(boundaries[1].getConnectedCells().size(), heightCellCount);
      EXPECT_EQ(boundaries[2].getConnectedCells().size(), widthCellCount);
      EXPECT_EQ(boundaries[3].getConnectedCells().size(), widthCellCount);
    }
  }
}

/**
 * Tests that the outflow boundary deletes particles in the halo area
 * and only in that area. It also ensures particles are deleted correctly.
 */
TEST(LinkedCellsBoundaryOutflow, Deletion2D) {
  double const meshWidth = 2.0;

  v3d dimensions{5, 4, 1};
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{meshWidth * dimensions};

  LinkedCellsContainer container{meshWidth, leftCorner, rightCorner};

  container.setBoundaries({{CubeSide::LEFT, BoundaryType::OUTFLOW},
                           {CubeSide::RIGHT, BoundaryType::OUTFLOW},
                           {CubeSide::TOP, BoundaryType::OUTFLOW},
                           {CubeSide::BOTTOM, BoundaryType::OUTFLOW}});

  // Uniformly distributed particles; (Clang forced me to write it like this)
  double x{leftCorner[0] - 2};
  while (x <= rightCorner[0] + 2) {
    double y = leftCorner[1] - 2;
    while (y <= rightCorner[1] + 2) {
      container.emplace_back(v3d{x, y, 0.5}, v3d{0, 0, 0}, 1, 0);
      y += 0.4;
    }
    x += 0.4;
  }

  container.recalculateStructure();

  // Helper to count particles in container and in halo
  auto countParticles = [&container] {
    size_t countedSize{};
    size_t countedHaloSize{};
    for (auto& cell : container.getCellsVector()) {
      if (cell.type == CellType::halo) countedHaloSize += cell.particles.size();

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

  EXPECT_EQ(containerSizeBefore, countedSize) << "It's possible that some particles are not linked or linked multiple "
                                                 "times.";

  // Apply boundaries, which should delete the halo
  container.applyBoundaries();
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
  EXPECT_NE(container.size(), 0) << "The boundaries deleted everything not just halo cell content.";

  // Check if .foreach() works correctly
  container.forEach(count);
  size_t forEachCountAfter = foreachCount;
  EXPECT_GT(forEachCountBefore, forEachCountAfter) << "Foreach must not apply to deleted particles.";
}

/**
 * Check if outflow boundary clears halo correctly and nothing else in 3D.
 * Also checks the internal boundary structure.
 */
TEST(LinkedCellsBoundaryOutflow, Deletion3D) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{10, 10, 10.};
  LinkedCellsContainer container{2., leftCorner, rightCorner};

  // Spawn test particles (clang complains when using for loops)
  size_t addedCount{};
  double cubeInitCoord = -1.0;
  double particleMeshWidth = 0.6;
  double x{cubeInitCoord};
  while (x < rightCorner[0] - cubeInitCoord) {
    double y{cubeInitCoord};
    while (y < rightCorner[1] - cubeInitCoord) {
      double z{cubeInitCoord};
      while (z < rightCorner[2] - cubeInitCoord) {
        container.emplace_back(v3d{x, y, z}, v3d{0, 0, 0}, 1, 0);
        addedCount++;
        z += particleMeshWidth;
      }
      y += particleMeshWidth;
    }
    x += particleMeshWidth;
  }

  // Measure particle counts in halo and within container
  size_t nonHaloParticlesBefore{};
  size_t haloParticlesBefore{};

  for (auto& cell : container.getCellsVector()) {
    if (cell.type == CellType::halo) {
      haloParticlesBefore += cell.particles.size();
    } else {
      nonHaloParticlesBefore += cell.particles.size();
    }
  }

  EXPECT_NE(nonHaloParticlesBefore, 0) << "For the test, some particles should be in inner/boundary cells.";

  ASSERT_EQ(addedCount, nonHaloParticlesBefore + haloParticlesBefore) << "The container lost particles.";

  ASSERT_NE(haloParticlesBefore, 0) << "To test deletion, ensure halo is not empty.";

  // Spawn and apply boundaries
  container.setBoundaries({{CubeSide::LEFT, BoundaryType::OUTFLOW},
                           {CubeSide::RIGHT, BoundaryType::OUTFLOW},
                           {CubeSide::TOP, BoundaryType::OUTFLOW},
                           {CubeSide::BOTTOM, BoundaryType::OUTFLOW},
                           {CubeSide::FRONT, BoundaryType::OUTFLOW},
                           {CubeSide::BACK, BoundaryType::OUTFLOW}});

  for (auto& b : container.getBoundaries()) {
    // Check internal structure is still correct
    for (auto& cell : b.getConnectedCells()) EXPECT_EQ(cell->type, CellType::boundary);

    EXPECT_EQ(b.getConnectedCells().size(), 25);
  }

  container.applyBoundaries();
  container.recalculateStructure();

  // Check new particle counts
  size_t nonHaloParticlesAfter{};
  size_t haloParticlesAfter{};

  for (auto& cell : container.getCellsVector()) {
    if (cell.type == CellType::halo) {
      haloParticlesAfter += cell.particles.size();
    } else {
      nonHaloParticlesAfter += cell.particles.size();
    }
  }

  EXPECT_EQ(nonHaloParticlesBefore, nonHaloParticlesAfter)
      << "Particles that where not in halo cells got deleted/appeared.";
  EXPECT_EQ(haloParticlesAfter, 0) << "Not all particles in halo cells where deleted.";
}

class LinkedCellsReflectiveBoundary : public ::testing::Test {
 protected:
  double epsilon{};
  double centerOffset{};
  std::vector<v3d> testParticles;

  void SetUp() override {
    epsilon = ReflectiveBoundary::SIXTH_ROOT_OF_2 / 5.0;
    centerOffset = 5.0 - epsilon;

    testParticles = TestUtils::getTestPositions(centerOffset);
  }
};

/**
 * Checks that particles are actually reflected exactly how they are supposed
 * to be reflected from walls.
 */
TEST_F(LinkedCellsReflectiveBoundary, ForcesCorrect) {
  v3d leftCorner{-5.0, -5.0, -5.0};
  v3d rightCorner{5.0, 5.0, 5.0};

  LinkedCellsContainer container{10.0, leftCorner, rightCorner};
  container.setBoundaries({{CubeSide::LEFT, BoundaryType::REFLECT},
                           {CubeSide::RIGHT, BoundaryType::REFLECT},
                           {CubeSide::TOP, BoundaryType::REFLECT},
                           {CubeSide::BOTTOM, BoundaryType::REFLECT},
                           {CubeSide::FRONT, BoundaryType::REFLECT},
                           {CubeSide::BACK, BoundaryType::REFLECT}});

  for (auto particleX : testParticles) container.emplace_back(particleX, {0.0, 0.0, 0.0}, 1.0, 0.0);

  container.applyBoundaries();

  std::vector<Particle> transformedParticles{};
  std::function copyParticles{[&transformedParticles](Particle& p) { transformedParticles.push_back(p); }};
  container.forEach(copyParticles);

  ASSERT_EQ(testParticles.size(), transformedParticles.size())
      << "Particle count unexpectedly changed when applying reflective "
         "boundary.";

  // The force that now applies to each of the particles now should face towards
  // the center coordinate. Thanks to the test setup is suffices to just check
  // whether forces and location share opposite signs.

  for (auto& particle : transformedParticles) {
    // Figure out signs of forces + location per dimension
    v3d positionSigns = particle.getX();
    v3d invForceSigns = (-1.0) * particle.getF();

    for (int i = 0; i < 3; i++) {
      positionSigns[i] = signum(positionSigns[i]);
      invForceSigns[i] = signum(invForceSigns[i]);
    }

    EXPECT_GT(ArrayUtils::L2Norm(invForceSigns), 0) << "No forces where applied onto particle.";

    if (ArrayUtils::L2Norm(invForceSigns) != 0.0) {
      EXPECT_EQ(positionSigns, invForceSigns) << "Particle is not symmetrically reflected from boundaries.";
    }

    double const expected = 7932629.5583620109;
    for (int i = 0; i < 3; i++) {
      auto const& f = particle.getF()[i];
      bool correctAmountOfForce = (f == expected || f == -expected || f == 0);

      EXPECT_TRUE(correctAmountOfForce) << "Where force value: " << f << '\n'
                                        << "expected values: {" << expected << ", " << (-expected) << ", 0.0}";
    }
  }
}

/**
 * Ignore particles that are outside of reflection distance.
 */
TEST_F(LinkedCellsReflectiveBoundary, IgnoreOutOfRange) {
  v3d leftCorner{-8.0, -8.0, -8.0};
  v3d rightCorner{8.0, 8.0, 8.0};

  LinkedCellsContainer container{4.0, leftCorner, rightCorner};
  container.setBoundaries({{CubeSide::LEFT, BoundaryType::REFLECT},
                           {CubeSide::RIGHT, BoundaryType::REFLECT},
                           {CubeSide::TOP, BoundaryType::REFLECT},
                           {CubeSide::BOTTOM, BoundaryType::REFLECT},
                           {CubeSide::FRONT, BoundaryType::REFLECT},
                           {CubeSide::BACK, BoundaryType::REFLECT}});

  // Apply boundaries
  container.applyBoundaries();

  // Get particles back
  std::vector<Particle> transformedParticles;
  std::function getParticles{[&transformedParticles](Particle& p) { transformedParticles.push_back(p); }};

  // All particles where far away from the boundary. No forces should apply.
  container.forEach(getParticles);

  for (auto& particle : transformedParticles) {
    ASSERT_EQ(particle.getF()[0], 0.0);
    ASSERT_EQ(particle.getF()[1], 0.0);
    ASSERT_EQ(particle.getF()[2], 0.0);
  }
}

/**
 * Holds values for periodic boundary tests
 */
class LinkedCellsBoundaryPeriodic : public ::testing::Test {
 protected:
  double const centerOffset = 5.0;
  double const epsilon = 0.05;
  double const within = centerOffset - epsilon;
  double const outside = centerOffset + epsilon;
};

/**
 * Periodic Boundary: Do not teleport particles that are inside the LinkedCells container (not in the halo area)
 */
TEST_F(LinkedCellsBoundaryPeriodic, NoTeleportation) {
  std::vector<v3d> testPositionsNoTeleport = TestUtils::getTestPositions(within);

  // Do not teleport if within boundary cells
  LinkedCellsContainer container = TestUtils::getBoundaryTestContainer(centerOffset, BoundaryType::PERIODIC);
  std::vector<Particle> noTpParticles = TestUtils::applyBoundariesReturnParticles(container, testPositionsNoTeleport);

  for (size_t i = 0; i < testPositionsNoTeleport.size(); i++) {
    EXPECT_EQ(noTpParticles.at(i).getX(), testPositionsNoTeleport.at(i));
  }
}

/**
 * Periodic Boundary: Do teleport particles that are outside the LinkedCells container (in the halo area) back inside
 */
TEST_F(LinkedCellsBoundaryPeriodic, Teleportation) {
  std::vector<v3d> testPositionsTeleport = TestUtils::getTestPositions(outside);

  // Do teleport if outside boundary cells, in halo cells
  LinkedCellsContainer container = TestUtils::getBoundaryTestContainer(centerOffset, BoundaryType::PERIODIC);
  std::vector<Particle> tpParticles = TestUtils::applyBoundariesReturnParticles(container, testPositionsTeleport);

  for (size_t i = 0; i < testPositionsTeleport.size(); i++) {
    v3d expectedTpLocation = testPositionsTeleport.at(i);
    for (size_t j = 0; j < 3; j++) {
      if (expectedTpLocation[j] == 0.0 || expectedTpLocation[j] == -0.0) {
        expectedTpLocation[j] = 0.0;
        continue;
      }

      if (expectedTpLocation[j] > 0.0) {
        expectedTpLocation[j] = -within;
        continue;
      }

      expectedTpLocation[j] = within;
    }

    auto actualLocation = tpParticles.at(i).getX();
    EXPECT_EQ(actualLocation, expectedTpLocation)
        << "Particle at index " << i << " was not teleported to its correct location.";
  }
}

/**
 * Periodic boundaries project boundary cells into halo cells.
 */
TEST_F(LinkedCellsBoundaryPeriodic, ForcesThrough3DCorner) {
  LinkedCellsContainer container = TestUtils::getBoundaryTestContainer(centerOffset, BoundaryType::PERIODIC);
  std::vector<v3d> locations{{within, within, within}, {-within, -within, -within}};
  std::vector<Particle> particles = TestUtils::applyBoundariesReturnParticles(container, locations);

  ASSERT_NE(particles.size(), 0) << "The container lost the particles we put into it.";

  for (auto& particle : particles) {
    for (size_t i = 0; i < 3; i++) {
      EXPECT_NE(particle.getF()[i], 0.0) << "No forces where applied in dimension " << i << ".";
    }
  }

  EXPECT_EQ(particles.at(0).getF(), (-1.0) * particles.at(1).getF()) << "Forces where not applied symmetrically";
  auto expected = v3d{-1097378875171.5186, -1097378875171.5186, -1097378875171.5186};
  for (int i = 0; i < 3; ++i) {
    EXPECT_DOUBLE_EQ(particles.at(0).getF()[i], expected[i]);
  }
}

/**
 * Checks 26 base cases where forces are applied through periodic boundaries.
 */
TEST_F(LinkedCellsBoundaryPeriodic, ForceBigCube) {
  // Get test cube with all test cases. Care: The test positions must not be too close to the border of the LinkedCells
  // container or the particles will repell each other.
  std::vector<v3d> testParticles = TestUtils::getTestPositions(centerOffset * 10 - 1.2);
  LinkedCellsContainer container =
      TestUtils::getBoundaryTestContainer(centerOffset * 10, 2 * centerOffset, BoundaryType::PERIODIC);
  std::vector<Particle> particles = TestUtils::applyBoundariesReturnParticles(container, testParticles);

  // If the periodic boundary works, all forces point outwards and on each dimension have the same magnitude or 0.
  for (auto& particle : particles) {
    // Because the sample cube is symmetric and centered at (0,0,0) we can translate positions to forces easily
    double const expectedMagnitude = 0.25890084470184332;
    auto const expectedForce = expectedMagnitude * signum(particle.getX());

    // Corner particles have 8 neighbors. Normally this means that the forces on them differ from the expectedMagnitude
    // above, as in these corner diagonal forces start to appear. In this test however, the test positions are chosen
    // in a way, where those diagonal particles, are beyond the cutoff limit and thus can be ignored, which means the
    // magnitude above is still valid for corner particles.
    EXPECT_EQ(particle.getF(), expectedForce);
  }
}