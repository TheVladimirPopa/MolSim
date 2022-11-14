#pragma once
#include "dataStructures/ParticleContainer.h"

namespace TestUtils {

  /**
   * Generates container for the ParticleContainer test suite.
   */
  ParticleContainer makeContainer(unsigned size) {
    ParticleContainer particle_container{};
    for (unsigned i = 0; i < size; i++) {
      Particle p{};
      particle_container.push_back(p);
    }

    return particle_container;
  }
}  // end namespace TestUtils
