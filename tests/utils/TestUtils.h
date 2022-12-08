#pragma once
#include "dataStructures/VectorContainer.h"

namespace TestUtils {

/**
 * Generates container for the ParticleContainer test suite.
 */
VectorContainer makeContainer(unsigned size) {
  VectorContainer particle_container{};
  for (unsigned i = 0; i < size; i++) {
    particle_container.emplace_back(std::array<double, 3>{},
                                    std::array<double, 3>{}, 1.0, 1);
  }

  return particle_container;
}
}  // end namespace TestUtils
