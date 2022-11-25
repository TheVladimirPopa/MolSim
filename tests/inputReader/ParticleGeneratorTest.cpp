#include "gtest/gtest.h"
#include "inputReader/ParticleGenerator.h"

using ParticleGeneration::addCuboidToParticleContainer;
using ParticleGeneration::cuboid;
using ParticleGeneration::sphere;

/**
 * Make sure there is a particle at each expected position
 */
TEST(ParticleGenerationCuboid, Position) {
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
TEST(ParticleGenerationCuboid, ParticleCount) {
  cuboid test{{0., 0., 0.}, {3, 4, 5}, {-10., -20., -30.}, 5, 42};
  ParticleContainer container{};

  addCuboidToParticleContainer(container, test);

  EXPECT_EQ(container.size(), 3 * 4 * 5)
      << "Particle count is not according to position";
}

/**
 * Make sure the mass of each particle is set correctly
 */
TEST(ParticleGenerationCuboid, Mass) {
  cuboid test{{0., 0., 0.}, {3, 4, 5}, {-10., -20., -30.}, 5, 42};
  ParticleContainer container{};

  addCuboidToParticleContainer(container, test);
  for (auto &particle : container) {
    ASSERT_EQ(particle.getM(), 42.0) << "Particle mass is not set correctly";
  }
}

/**
 * Make sure particles are on their correct locations
 */
TEST(ParticleGenerationSphere, Position) {
  sphere test{{0., 0., 0.}, 2, 2, {-10., -20., -30.}, 4, 42, 0};
  ParticleContainer container{};

  addSphereToParticleContainer(container, test);

  std::vector<std::array<double, 3>> expected{
      {2.0, 2.0, 0.0}, {-2.0, 2.0, 0.0}, {2.0, -2.0, 0.0}, {-2.0, -2.0, 0.0},
      {2.0, 6.0, 0.0}, {-2.0, 6.0, 0.0}, {2.0, -6.0, 0.0}, {-2.0, -6.0, 0.0},
      {6.0, 2.0, 0.0}, {-6.0, 2.0, 0.0}, {6.0, -2.0, 0.0}, {-6.0, -2.0, 0.0}};

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
 * Make sure there are as many particles generated as specified by the radius
 */
TEST(ParticleGenerationSphere, ParticleCount) {
  sphere test{{0., 0., 0.}, 2, 2, {-10., -20., -30.}, 4, 42, 0};
  ParticleContainer container{};

  addSphereToParticleContainer(container, test);

  EXPECT_EQ(container.size(), 12) << "Particle count of 2d circle is wrong";

  sphere test_3d{{0., 0., 0.}, 2, 3, {-10., -20., -30.}, 4, 42, 0};
  ParticleContainer container_3d{};
  addSphereToParticleContainer(container_3d, test_3d);
  EXPECT_EQ(container_3d.size(), 4 + 12 + 12 + 4)
      << "Particle count of 3d sphere is wrong";
}

/**
 * This test makes sure that for high input values the algorithm still
 * produces a correct result. (I.e. rounding will be no problem in the
 * simulation.)
 */
TEST(ParticleGenerationSphere, ParticleCountHighValues) {
  sphere test_3d{{1e9, 1e9, 1e9}, 3, 3, {1., 5., 3.}, 10000.0, 42, 0};
  ParticleContainer container_3d{};
  addSphereToParticleContainer(container_3d, test_3d);
  EXPECT_EQ(container_3d.size(), 136) << "Particle count of 3d sphere is wrong";
}

/**
 * Make sure the mass of each particle is set correctly
 */
TEST(ParticleGenerationSphere, Mass) {
  sphere test{{0., 0., 0.}, 2, 2, {-10., -20., -30.}, 4, 42, 0};
  ParticleContainer container{};

  addSphereToParticleContainer(container, test);
  for (auto &particle : container) {
    ASSERT_EQ(particle.getM(), 42.0) << "Particle mass is not set correctly";
  }
}
