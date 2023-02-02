#include "dataStructures/Particle.h"
#include "gtest/gtest.h"
#include "utils/ThermodynamicStatistics.h"
#define PI 3.14159265358979323846

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

TEST(ThermodynamicStatisticsTest, RDFTestDensity) {
  VectorContainer container{};

  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{2.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p3{{3.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p4{{4.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  container.push_back(p1);
  container.push_back(p2);
  container.push_back(p3);
  container.push_back(p4);

  std::vector<double> result = ThermodynamicStatistics::calculateRadialDistributionFunction(1, &container, 1, 4);
  std::vector<double> expected = {6. / (28. * PI), 3. / (76. * PI), 3. / (148. * PI), 0.};

  for (int i = 0; i < 4; i++) {
    EXPECT_NEAR(result[i], expected[i], 0.00000001);
  }
}