#include "ParticleContainerTest.h"
#include "dataStructures/ParticleContainer.h"
#include "gtest/gtest.h"

TEST(ForEachTestSuite, AppliesToAllElements) {
  ParticleContainer particleContainer{};

  std::array<double, 3> x_arg = {0.0, 0.0, 0.0};
  std::array<double, 3> v_arg = {0.0, 0.0, 0.0};
  double m_arg = 1.0;
  int type = 0;

  for (int i = 0; i < 50; i++) {
    Particle p{x_arg, v_arg, m_arg, type};
    particleContainer.push_back(p);
  }

  int ct = 0;
  std::function<void(Particle &)> count =
      [&ct](auto & p){ ct += 1; };

  particleContainer.forEach(count);

  ASSERT_EQ(ct, 50);
}