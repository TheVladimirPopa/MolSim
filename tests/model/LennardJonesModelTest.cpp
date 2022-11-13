#include <cmath>

#include "gtest/gtest.h"
#include "model/LennardJonesModel.h"
#include "model/NewtonsLawModel.h"
using std::pow;
#include "climits"

/**
 * Worksheet 2 defines epsilon = 5.0 and sigma = 1.0 for the Lennard-Jones
 * potential. (This test might get removed in the future.)
 */
TEST(LennardJonesModel, WorksheetParameters) {
  LennardJonesModel model{};

  EXPECT_EQ(model.getEpsilon(), 5.0) << "Worksheet 2 expects epsilon = 5.0";
  EXPECT_EQ(model.getSigma(), 1.0) << "Worksheet 2 expects sigma = 1.0";
}

/**
 * Check if the applied force has the correct sign on each particle.
 * This tests uses NewtonsLawModel as a assumed correct reference.
 */
TEST(LennardJonesModel, ForceCorrectSign) {
  // Use NewtonsLawModel as reference as it already works.
  NewtonsLawModel modelNewton{};
  Particle pNewton1{{0.0, 0.0, 0.0}};
  Particle pNewton2{{1.0, 0.0, 0.0}};
  modelNewton.addForces(pNewton1, pNewton2);

  LennardJonesModel model{};
  Particle pLennard1{{0.0, 0.0, 0.0}};
  Particle pLennard2{{1.0, 0.0, 0.0}};
  model.addForces(pLennard1, pLennard2);

  EXPECT_NE(pLennard1.getF()[0], 0.0) << "Applied force is zero.";
  EXPECT_NE(pLennard2.getF()[0], 0.0) << "Applied force is zero.";

  EXPECT_EQ(pNewton1.getF()[0] > 0, pLennard1.getF()[0] > 0)
      << "Force direction is different";
  EXPECT_EQ(pNewton2.getF()[0] > 0, pLennard2.getF()[0] > 0)
      << "Force direction is different";
}

/**
 * Check if model calculates the correct value in an example where
 * the resulting force is already known.
 */
TEST(LennardJonesModel, ForcePrecalculatedExample) {
  ParticleContainer c{};

  double distance = 2.0;
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  // Expected force via Wolfram Alpha. Formula:
  // (-(24 * 5)/2) * ((1/2)^6 - 2(1/2)^12) * {{2}, {0}, {0}}
  std::array<double, 3> expected_force = {-465.0 / 512.0, 0.0, 0.0};

  LennardJonesModel model{};
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
  EXPECT_NEAR(p1.getF()[0], expected_force[0],
              std::numeric_limits<double>::epsilon())
      << "Note: this last expect might assume unreasonable precision.";
}