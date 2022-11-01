//
// Created by leo on 01.11.22.
//

#include "ParticleContainer.h"

std::vector<Particle>& ParticleContainer::getVectorRef() { return vector; }

void ParticleContainer::forEach(std::function<void(Particle&)>& unaryFunction) {
  for (Particle& particle : vector) {
    unaryFunction(particle);
  }
}

void ParticleContainer::forEachPair(
    std::function<void(Particle&, Particle&)>& binaryFunction) {
  for (size_t i = 0; i < vector.size(); ++i) {
    for (size_t j = i + 1; j < vector.size(); ++j) {
      binaryFunction(vector[i], vector[j]);
    }
  }
}