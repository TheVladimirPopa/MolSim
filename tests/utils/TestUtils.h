#pragma once
#include "dataStructures/ParticleContainer.h"

namespace TestUtils {

  /**
   * Generates container for the ParticleContainer test suite.
   */
  static ParticleContainer makeContainer(unsigned size) {
    std::array<double, 3> x_arg = {0.0, 0.0, 0.0};
    std::array<double, 3> v_arg = {0.0, 0.0, 0.0};
    double m_arg = 1.0;
    int type = 0;

    ParticleContainer particle_container{};
    for (unsigned i = 0; i < size; i++) {
      Particle p{x_arg, v_arg, m_arg, type};
      particle_container.push_back(p);
    }

    return particle_container;
  }
}

