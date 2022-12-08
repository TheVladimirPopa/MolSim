#include "dataStructures/LinkedCellsContainer.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"

using v3d = std::array<double, 3>;

/**
 * Make sure the dimension of the cells are correct and the cell types are
 * correct
 */
TEST(LinkedCellsContainer, SetupDim) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{10., 8., 6.};
  LinkedCellsContainer c{2.0, leftCorner, rightCorner};
  auto cells = c.getCellsVector();

  EXPECT_EQ(cells.size(), 7 * 6 * 5);

  EXPECT_EQ(cells[0].type, cellType::halo);

  EXPECT_EQ(cells[1 + 7 + (7 * 6)].type, cellType::boundary);

  int counter = 0;
  for (auto &cell : cells) {
    if (cell.type != cellType::halo) {
      counter++;
    }
  }
  EXPECT_EQ(counter, 5 * 4 * 3);
}

/**
 * Make sure each particle is placed in the correct cell when emplaced_back
 */
TEST(LinkedCellsContainer, ParticlePlacementInDomain) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{8., 8., 6.};
  LinkedCellsContainer c{1.0, leftCorner, rightCorner};

  c.emplace_back(v3d{0., 0., 0.}, v3d{0., 0., 0.}, 1., 1);

  for (size_t i = 0; i < c.size(); ++i) {
    auto &cell = c.getCellsVector()[i];
    if (i == 1 + 10 + 100) {
      EXPECT_EQ(cell.particles.size(), 1);
    } else {
      EXPECT_EQ(cell.particles.size(), 0);
    }
  }

  c.emplace_back(v3d{5., 4., 3.}, v3d{0., 0., 0.}, 1., 2);

  int correctIndex = 6 + (5 * 10) + (4 * 100);
  EXPECT_EQ(c.getCellsVector()[correctIndex].particles.size(), 1);
}

/**
 * Make sure the particles are emplaced in the correct halo cell when outside
 * the domain boundary
 */
TEST(LinkedCellsContainer, ParticlePlacementOutsideDomain) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{8., 6., 4.};
  LinkedCellsContainer c{1.0, leftCorner, rightCorner};

  c.emplace_back(v3d{-20., -3., -1.1}, v3d{0., 0., 0.}, 1., 1);

  EXPECT_EQ(c.getCellsVector()[0].particles.size(), 1);

  c.emplace_back(v3d{9., 7., 5.}, v3d{0., 0., 0.}, 1., 1);

  EXPECT_EQ(c.getCellsVector()[c.getCellsVector().size() - 1].particles.size(),
            1);
}

/**
 * Make sure placement is also correct when leftCorner is not 0 0 0
 */
TEST(LinkedCellsContainer, BoundingBoxOffset) {
  v3d leftCorner{-4., -6., -8.};
  v3d rightCorner{8., 6., 4.};
  LinkedCellsContainer c{2.0, leftCorner, rightCorner};
  c.emplace_back(v3d{1., 1., 1.}, v3d{0., 0., 0.}, 1., 1);

  int correctIndex = 3 + (8 * 4) + (8 * 8 * 5);
  EXPECT_EQ(c.getCellsVector()[correctIndex].particles.size(), 1);
}

/**
 * Make sure the restructure places the particles correctly when calling
 * foreachPair
 */
TEST(LinkedCellsContainer, ForEachRestructure) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{8., 8., 6.};
  LinkedCellsContainer c{1.0, leftCorner, rightCorner};

  c.emplace_back(v3d{0., 0., 0.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{0., 5., 0.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{1., 4., 0.}, v3d{0., 0., 0.}, 1., 2);

  std::function moveX = [](Particle &p) {
    if (p.getType() == 2) return;
    auto newPos = p.getX() + v3d{1., 0., 0.};
    p.setX(newPos);
  };
  c.forEach(moveX);

  std::function nothing = [&](Particle &, Particle &) {};
  c.forEachPair(nothing);
  EXPECT_EQ(c.getCellsVector()[100 + 10 + 1].particles.size(), 0);
  EXPECT_EQ(c.getCellsVector()[100 + 60 + 1].particles.size(), 0);

  EXPECT_EQ(c.getCellsVector()[100 + 10 + 2].particles.size(), 1);
  EXPECT_EQ(c.getCellsVector()[100 + 60 + 2].particles.size(), 1);

  std::function moveY = [](Particle &p) {
    if (p.getType() == 1) return;
    auto newPos = p.getX() + v3d{0., 1., 0.};
    p.setX(newPos);
  };
  c.forEach(moveY);
  c.forEachPair(nothing);

  EXPECT_EQ(c.getCellsVector()[100 + 60 + 2].particles.size(), 2);
  EXPECT_EQ(c.getCellsVector()[100 + 50 + 2].particles.size(), 0);

  c.forEach(moveY);
  c.forEachPair(nothing);

  EXPECT_EQ(c.getCellsVector()[100 + 60 + 2].particles.size(), 1);
  EXPECT_EQ(c.getCellsVector()[100 + 70 + 2].particles.size(), 1);
  EXPECT_EQ(c.getCellsVector()[100 + 10 + 2].particles.size(), 1);
}

/**
 * Make sure forEachPair builds correct paris for particles within one cell
 */
TEST(LinkedCellsContainer, ForEachOneCell) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{8., 8., 8.};
  LinkedCellsContainer c{4.0, leftCorner, rightCorner};

  c.emplace_back(v3d{0., 0., 0.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{0., 1., 0.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{0., 2., 0.}, v3d{0., 0., 0.}, 1., 2);
  c.emplace_back(v3d{1., 0., 0.}, v3d{0., 0., 0.}, 1., 2);
  c.emplace_back(v3d{2., 0., 0.}, v3d{0., 0., 0.}, 1., 2);

  int counter = 0;
  std::function countCalls = [&counter](Particle &, Particle &) { counter++; };
  c.forEachPair(countCalls);

  EXPECT_EQ(counter, 5 * (5 - 1) / 2);
}

/**
 * Make sure forEachPair builds correct pair with and only with adjacent cells
 */
TEST(LinkedCellsContainer, PairAdjacentParticlesSimple) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{16., 14., 10.};
  LinkedCellsContainer c{2.0, leftCorner, rightCorner};
  c.emplace_back(v3d{1., 1., 1.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{3., 1., 1.}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{9., 1., 1.}, v3d{0., 0., 0.}, 1., 1);

  int counter = 0;
  std::function countCalls = [&counter](Particle &, Particle &) { counter++; };
  c.forEachPair(countCalls);

  EXPECT_EQ(counter, 1);
}

/**
 * Make sure forEachPair builds correct Pairs with adjacent cells
 */
TEST(LinkedCellsContainer, PairAdjacentParticlesAdvanced) {
  v3d leftCorner{0., 0., 0.};
  v3d rightCorner{16., 14., 10.};
  LinkedCellsContainer c{1.0, leftCorner, rightCorner};
  c.emplace_back(v3d{5.5, 5.5, 5.5}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{5.5, 5.5, 5.5}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{6.5, 6.5, 6.5}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{4.5, 4.5, 4.5}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{4.5, 6.5, 6.5}, v3d{0., 0., 0.}, 1., 1);
  c.emplace_back(v3d{6.5, 4.5, 4.5}, v3d{0., 0., 0.}, 1., 1);

  int counter = 0;
  std::function countCalls = [&counter](Particle &, Particle &) { counter++; };
  c.forEachPair(countCalls);

  EXPECT_EQ(counter, 1 + (4 * 2));
}