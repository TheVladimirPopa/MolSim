#include "gtest/gtest.h"
#include "inputReader/ParticleGenerator.h"

using ParticleGeneration::addCuboidToParticleContainer;
using ParticleGeneration::cuboid;

/**
 * Make sure there is a particle at each expected position
 */
TEST(ParticleGeneration, Position) {
  cuboid test{{0., 0., 0.}, {2, 1, 2}, {-10., -20., -30.}, 5, 42};
  ParticleContainer container{};

  addCuboidToParticleContainer(container, test);

  std::vector<std::array<double, 3>> expected{
      {0., 0., 0.}, {5., 0., 0.}, {0., 0., 5.}, {5., 0., 5.}};

  for (auto &expectedPos : expected) {
    bool found = false;
    for (auto &c : container) {
      if (c.getX() == expectedPos) {
        found = true;
        break;
      }
    }
    EXPECT_TRUE(found) << "Particle positions not correct ";
  }
}

/**
 * Make sure there are as many particles generated as specified by the dimension
 */
TEST(ParticleGeneration, ParticleCount) {
  cuboid test{{0., 0., 0.}, {3, 4, 5}, {-10., -20., -30.}, 5, 42};
  ParticleContainer container{};

  addCuboidToParticleContainer(container, test);

  EXPECT_EQ(container.size(), 3 * 4 * 5)
      << "Particle count is not according to position";
}

/**
 * Make sure the mass of each particle is set correctly
 */
TEST(ParticleGeneration, Mass) {
  cuboid test{{0., 0., 0.}, {3, 4, 5}, {-10., -20., -30.}, 5, 42};
  ParticleContainer container{};

  addCuboidToParticleContainer(container, test);
  for (auto &particle : container) {
    ASSERT_EQ(particle.getM(), 42) << "Particle mass is not set correctly";
  }
}
