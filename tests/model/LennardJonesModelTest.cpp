#include <cmath>

#include "gtest/gtest.h"
#include "model/LennardJonesModel.h"
#include "model/NewtonsLawModel.h"
using std::pow;


/**
 * Check if the applied forces have the correct sign on each particle.
 */
TEST(LennardJonesModel, ForceCorrectSign) {
  LennardJonesModel model{100.};
  Particle p1{{0.0, 0.0, 0.0}};
  Particle p2{{1.0, 0.0, 0.0}};
  model.addForces(p1, p2);

  EXPECT_NE(p1.getF()[0], 0.0) << "Applied force is zero.";
  EXPECT_NE(p2.getF()[0], 0.0) << "Applied force is zero.";

  EXPECT_LT(p1.getF()[0], 0.0) << "Force on particle 1 points in wrong direction on the x0 axis.";
  EXPECT_GT(p2.getF()[0], 0.0) << "Force on particle 2 points in wrong direction on the x0 axis of.";
}

/**
 * Check if model calculates the correct value in an example where
 * the resulting force is already known.
 */
TEST(LennardJonesModel, ForcePrecalculatedExample) {
  VectorContainer c{};

  double distance = 2.0;
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  // Expected force via Wolfram Alpha. Formula:
  // (-(24 * 5)/2) * ((1/2)^6 - 2(1/2)^12) * {{2}, {0}, {0}}
  std::array<double, 3> expected_force = {465.0 / 512.0, 0.0, 0.0};

  LennardJonesModel model{100.};
  model.addForces(p1, p2);

  // Check if result is approximately correct
  EXPECT_NEAR(p1.getF()[0], expected_force[0], 0.1);
  EXPECT_EQ(p1.getF()[1], 0.0);
  EXPECT_EQ(p1.getF()[2], 0.0);

  // Check if forces are mirrored
  EXPECT_EQ(p1.getF(), (-1) * p2.getF()) << "F_ij should be equal to -F_ji.";

  // Check if result is correct enough for our purposes
  EXPECT_NEAR(p1.getF()[0], expected_force[0], 0.000001);
  EXPECT_NEAR(p1.getF()[0], expected_force[0], 0.00000000000001);
  EXPECT_NEAR(p1.getF()[0], expected_force[0], std::numeric_limits<double>::epsilon())
      << "Note: this last expect might assume unreasonable precision.";
}

/**
 * Check if Lorentz-Berthelot mixing rules are applied
 */
TEST(LennardJonesModel, ForceMixingRules) {

  VectorContainer c{};
  double distance = 2.0;
  Particle::registerParticleType(1, 99.0, 1.5);
  Particle::registerParticleType(2, 1.0, 0.5);
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 1.0, 1};
  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}, 2.0, 2};

  EXPECT_EQ(p1.getM(), 1.0);
  EXPECT_EQ(p1.getEpsilon(), 99.0);
  EXPECT_EQ(p1.getSigma(), 1.5);

  EXPECT_EQ(p2.getM(), 2.0);
  EXPECT_EQ(p2.getEpsilon(), 1.0);
  EXPECT_EQ(p2.getSigma(), 0.5);

  LennardJonesModel model{100.};
  model.addForces(p1, p2);

  // Epsilon mixing rule: sqrt(eps1 * eps2) = 10.0  (5.0 with default settings)
  // Sigma mixing rule: (sig1 + sig2)/2 = 1.0
  // => Expect 2 times the force of "ForcePrecalculatedExample" Test

  std::array<double, 3> expected_force = {2 * 465.0 / 512.0, 0.0, 0.0};

  for (size_t i = 0; i < 3; i++) {
    EXPECT_NEAR(p1.getF().at(i), expected_force.at(i), 0.01);
    EXPECT_NEAR(p2.getF().at(i), ((-1.0) * expected_force).at(i), 0.01);
  }
}