#include "dataStructures/ParticleContainer.h"
#include "gtest/gtest.h"

ParticleContainer makeContainer(unsigned size);

/**
 * Tests whether the behaviour of forEach and forEachPair
 * handles the empty container edge case.
 */
TEST(ParticleContainer, EmptyDoesNothing) {
  ParticleContainer particle_container{};
  unsigned ct = 0;
  std::function<void(Particle &)> count = [&ct](auto& p) { ct++; };
  particle_container.forEach(count);

  std::function<void(Particle &, Particle &)> countPair =
      [&ct](auto& p1, auto& p2) { ct++; };
  particle_container.forEachPair(countPair);

  EXPECT_EQ(ct, 0);
}

/**
 * Tests whether a function is applied to all elements of
 * a particle container.
 */
TEST(ParticleContainer, ForEachBasic) {
  // Prepare container
  unsigned size = 50;
  ParticleContainer particle_container = makeContainer(size);

  // Make sure we don't lose elements or add something twice
  ASSERT_EQ(particle_container.size(), size);

  // Check whether a function is truly applied to all elements
  unsigned ct = 0;
  std::function<void(Particle &)> countAndModify = [&ct](auto& p){
        std::array<double, 3> test_value = {static_cast<double>(ct), 0.0, 0.0};
        p.setX(test_value);
        ct++;
      };

  particle_container.forEach(countAndModify);

  EXPECT_EQ(ct, size);

  for (unsigned i = 0; i < size; i++) {
    // Check whether the correct particles where modified
    EXPECT_EQ(particle_container[i].getX()[0], static_cast<double>(i));
  }
}

/**
 * Tests whether a function is applied to all pairs of particles exactly once
 */
TEST(ParticleContainer, ForEachPairBasic) {
  // Prepare container
  unsigned size = 49;
  ParticleContainer particle_container = makeContainer(size);

  ASSERT_GE(size, 1) << "To test pairs we need at least 2 particles.";

  unsigned ct = 0;
  std::function<void(Particle &, Particle &)> countPair =
      [&ct](auto& p1, auto& p2) { ct++; };
  particle_container.forEachPair(countPair);

  EXPECT_EQ(ct, size * (size-1) / 2) << "Number of pairs wrong.";

  // Set all x coordinates with a dummy value
  std::function<void(Particle &, Particle &)> setDummyX =
      [](auto& p1, auto& p2) {
        std::array<double, 3> dummy_x1 = {p1.getX()[0] + 1, 0.0, 0.0};
        std::array<double, 3> dummy_x2 = {p1.getX()[0] + 1, 0.0, 0.0};

        p1.setX(dummy_x1);
        p2.setX(dummy_x2);
      };

  particle_container.forEachPair(setDummyX);

  for (const auto &p : particle_container)
    ASSERT_EQ(p.getX()[0], size-1)
        << "ForEachPair did not process each pair exactly once.";
}


/**
 * Generates container for the ParticleContainer test suite.
 */
ParticleContainer makeContainer(unsigned size) {
  std::array<double, 3> x_arg = {0.0, 0.0, 0.0};
  std::array<double, 3> v_arg = {0.0, 0.0, 0.0};
  double m_arg = 1.0;
  int type = 0;

  ParticleContainer particle_container{};
  for (unsigned i = 0; i < size; i++) {
    Particle p{x_arg, v_arg, m_arg, type};
    particle_container.push_back(p);
  }

  return particle_container;
}