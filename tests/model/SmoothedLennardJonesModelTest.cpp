#include <cmath>

#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"
#include "gtest/gtest.h"
#include "model/SmoothedLennardJonesModel.h"

/**
 * Check if the SmoothedLennardJonesModel is applied when the particle distance is between the cutoffRadius and radius_l
 */
TEST(SmoothedLennardJonesModel, ForceNew) {
  VectorContainer c{};

  double distance = 2.1;
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  std::array<double, 3> expected_force = {0.56649460220233661 * 2.1, 0.0, 0.0};

  SmoothedLennardJonesModel model{2.3, 1.9};

  model.addForces(p1, p2);

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
 * Check if the regulat LennardJones Potential is used for distance <= radius_l
 */
TEST(SmoothedLennardJonesModel, ForceOld) {
  VectorContainer c{};

  double distance = 2.0;
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  std::array<double, 3> expected_force = {465.0 / 512.0, 0.0, 0.0};

  SmoothedLennardJonesModel model{2.3, 2.1};
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
 * Check if the applied forces are 0 when the particles are too far apart (beyond the cutoffRadius)
 */

TEST(SmoothedLennardJonesModel, ForceNewNull) {
  VectorContainer c{};

  double distance = 3.0;
  Particle p1{{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  Particle p2{{distance, 0.0, 0.0}, {0.0, 0.0, 0.0}};

  std::array<double, 3> expected_force = {0.0, 0.0, 0.0};

  SmoothedLennardJonesModel model{2.3, 1.9};
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