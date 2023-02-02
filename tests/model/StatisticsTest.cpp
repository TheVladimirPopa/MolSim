#include "gtest/gtest.h"
#include "utils/ThermodynamicStatistics.h"

ThermodynamicStatistics statistics{};

TEST(ThermodynamicStatisticsTest, VarianceTest) {
  VectorContainer container{};

  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{4.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p3{{8.3, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p4{{9.1, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  p1.setLastPosition({1., 3., 4.});
  p2.setLastPosition({2., 3., 5.});
  p3.setLastPosition({1., 2., 6.});
  p4.setLastPosition({5., 0., -3});

  container.push_back(p1);
  container.push_back(p2);
  container.push_back(p3);
  container.push_back(p4);

  double variance = statistics.calculateVariance(&container);
  double expectedVariance = 45.775;
  EXPECT_NEAR(variance, expectedVariance, 0.00000001);
}