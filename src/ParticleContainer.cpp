//
// Created by leo on 01.11.22.
//

#include "ParticleContainer.h"

void ParticleContainer::forEach(std::function<void(Particle&)>& unaryFunction) {
  for (int i = 0; i < size(); ++i) {
    unaryFunction(this->operator[](i));
  }
}

void ParticleContainer::forEachPair(
    std::function<void(Particle&, Particle&)>& binaryFunction) {
  for (size_t i = 0; i < size(); ++i) {
    for (size_t j = i + 1; j < size(); ++j) {
      binaryFunction(this->operator[](i), this->operator[](j));
    }
  }
}