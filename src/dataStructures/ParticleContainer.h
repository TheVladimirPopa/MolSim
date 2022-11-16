//
// Created by leo on 01.11.22.
//
#pragma once
#include <vector>

#include "Particle.h"

class ParticleContainer : public std::vector<Particle> {
 public:
  /**
   * Applies a function to all particles in the container
   * @param unaryFunction A function taking a particle reference and returning
   * nothing
   */
  template <typename F>
  void forEach(F unaryFunction) {
    for (size_t i = 0; i < size(); ++i) {
      unaryFunction(this->operator[](i));
    }
  }

  /**
   * Applies a function to all particle pairs in the container
   * @param binaryFunction A function taking two particle references and
   * returning nothing
   * @note A pair is considered without order, so if the function got applied to
   * (p1,p2), it won't get applied to (p2,p1). Also pairs with the same particle
   * are not considered
   */
  template <typename F>
  void forEachPair(F binaryFunction) {
    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = i + 1; j < size(); ++j) {
        binaryFunction(this->operator[](i), this->operator[](j));
      }
    }
  }
};
