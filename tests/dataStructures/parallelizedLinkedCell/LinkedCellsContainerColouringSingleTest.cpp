#include "dataStructures/parallelizedLinkedCell/LinkedCellsContainerColouringSingle.h"
#include "gtest/gtest.h"
#include "inputReader/ParticleGenerator.h"
#include "utils/TestUtils.h"

using v3d = std::array<double, 3>;

/**
 * Make sure each cell gets processed, but within each colour no race conditions can occur
 */
TEST(LinkedCellsContainerColouringSingle, testAll) {
  auto left = v3d{0., 0., 0.};
  auto right = v3d{5., 6., 4.};
  LinkedCellsContainerColouringSingle container{1.0, left, right};

  std::vector<bool> locks = *new std::vector<bool>(container.getCellsVector().size());

  // Make sure no race conditions can be raised
  for (auto &colour : container.getColourList()) {
    for (size_t i = 0; i < locks.size(); ++i) {
      locks[i] = false;
    }

    for (auto index : colour) {
      for (size_t offset : container.indexOffsetAdjacent) {
        ASSERT_FALSE(locks[index + offset]) << index + offset << " was accessed parallel";
        locks[index + offset] = true;
      }
    }
  }

  for (size_t i = 0; i < locks.size(); ++i) {
    locks[i] = false;
  }

  // Make sure each index gets called
  for (auto &colour : container.getColourList()) {
    for (auto index : colour) {
      locks[index] = true;
    }
  }

  for (size_t i = 0; i < locks.size(); ++i) {
    ASSERT_TRUE(locks[i] or container.getCellsVector()[i].type == CellType::halo) << "A cell didn't get processed";
  }
}

/**
 * Make sure, the particlePairs are equal to the normal linkedCellsContainer by checking if the forces are equal
 * It also checks, if periodic boundaries are still applied in a correct way
 */
TEST(LinkedCellsContainerColouringSingle, compareResultsToLinkedCells) {
  v3d left{0., 0., 0.};
  v3d right{7., 3., 2.5};
  LinkedCellsContainer linkedCellsContainer{1.5, left, right};
  LinkedCellsContainerColouringSingle colouringMultiple{1.5, left, right};

  TestUtils::setupLinkedCellsContainer(linkedCellsContainer);
  TestUtils::setupLinkedCellsContainer(colouringMultiple);

  TestUtils::applyForceCalc(linkedCellsContainer);
  TestUtils::applyForceCalc(colouringMultiple);

  auto particlesLinkedCells = TestUtils::getParticleVector(linkedCellsContainer);
  auto particlesMultipleColouring = TestUtils::getParticleVector(colouringMultiple);

  ASSERT_EQ(particlesLinkedCells.size(), particlesMultipleColouring.size());

  for (size_t i = 0; i < particlesMultipleColouring.size(); ++i) {
    Particle &a = particlesMultipleColouring[i];
    Particle &b = particlesLinkedCells[i];
    for (int j = 0; j < 3; ++j) {
      EXPECT_NEAR(a.getF()[j], b.getF()[j], 0.00000001);
    }
  }
}
