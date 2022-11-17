#include "gtest/gtest.h"
#include "inputReader/LineProcessorCuboid.h"

/**
 * Test if the dimension is read in correctly and therefore the particle count
 * has to be correct
 */
TEST(LineProcessorCuboid, ParticleCount) {
  LineProcessorCuboid lineProcessorCuboid{};
  std::istringstream dataStream(
      "0.0 0.0 0.0     0.0 0.0 0.0     40 8 1          1.1225          1.0     "
      "    1");
  ParticleContainer container{};
  lineProcessorCuboid.processLine(dataStream, container);

  EXPECT_EQ(40 * 8 * 1, container.size());
}

/**
 * Check the particle mass is read in and set correctly
 */
TEST(LineProcessorCuboid, ParticleMass) {
  LineProcessorCuboid lineProcessorCuboid{};
  std::istringstream dataStream(
      "0.0 0.0 0.0     0.0 0.0 0.0     2 3 1          1.1225          42.0     "
      "    1");
  ParticleContainer container{};
  lineProcessorCuboid.processLine(dataStream, container);

  for (auto &p : container) {
    EXPECT_EQ(42.0, p.getM());
  }
}

/**
 * Check the particle type is read in and set correctly
 */
TEST(LineProcessorCuboid, ParticleType) {
  LineProcessorCuboid lineProcessorCuboid{};
  std::istringstream dataStream(
      "0.0 0.0 0.0     0.0 0.0 0.0     4 3 2          1.1225          1.0      "
      "   42");
  ParticleContainer container{};
  lineProcessorCuboid.processLine(dataStream, container);

  for (auto &p : container) {
    EXPECT_EQ(42.0, p.getType());
  }
}
