#include "dataStructures/parallelizedLinkedCell/LinkedCellsContainerColouringMultiple.h"
#include "gtest/gtest.h"
#include "utils/TestUtils.h"

/**
 * Make sure each cell gets processed, but within each colour no race conditions can occur
 */
TEST(LinkedCellsContainerColouringMultiple, testAll) {
  v3d left{0., 0., 0.};
  v3d right{10., 5., 4.};
  std::vector<size_t> locks;

  LinkedCellsContainerColouringMultiple container{1.0, left, right};

  locks.resize(container.getCellsVector().size());

  // Make sure no race conditions can be raised
  for (auto &colour : container.getColourList()) {
    for (size_t i = 0; i < locks.size(); ++i) {
      locks[i] = 0;
    }

    for (size_t threadNum = 0; threadNum < colour.size(); ++threadNum) {
      for (auto index : colour[threadNum]) {
        if (container.getCellsVector()[index].type == CellType::halo) continue;

        for (size_t offset : container.indexOffsetAdjacent) {
          ASSERT_TRUE(locks[index + offset] == threadNum + 1 or locks[index + offset] == 0)
              << index + offset << " was accessed parallel, at thread " << threadNum + 1;
          locks[index + offset] = threadNum + 1;
        }
      }
    }
  }

  for (size_t i = 0; i < locks.size(); ++i) {
    locks[i] = 0;
  }

  // Make sure each index gets called
  for (auto &colour : container.getColourList()) {
    for (auto threadTask : colour) {
      for (auto index : threadTask) {
        locks[index] = 1;
      }
    }
  }

  for (size_t i = 0; i < locks.size(); ++i) {
    ASSERT_TRUE(locks[i] or container.getCellsVector()[i].type == CellType::halo) << "A cell didn't get processed";
  }
}

/**
 * Make sure, the particlePairs are equal to the normal linkedCellsContainer by checking if the forces are equal
 * It also checks, if periodic boundaries are still applied in a correct way.
 */
TEST(LinkedCellsContainerColouringMultiple, compareResultsToLinkedCells) {
  v3d left{0., 0., 0.};
  v3d right{7., 3., 2.5};
  LinkedCellsContainer linkedCellsContainer{1.5, left, right};
  LinkedCellsContainerColouringMultiple colouringMultiple{1.5, left, right};

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
