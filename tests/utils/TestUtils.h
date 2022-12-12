#pragma once
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"
using v3d = std::array<double, 3>;

namespace TestUtils {

/**
 * Generates container for the ParticleContainer test suite.
 */
VectorContainer makeContainer(unsigned size) {
  VectorContainer particle_container{};
  for (unsigned i = 0; i < size; i++) {
    particle_container.emplace_back(v3d{}, v3d{}, 1.0, 1);
  }

  return particle_container;
}
}  // end namespace TestUtils
