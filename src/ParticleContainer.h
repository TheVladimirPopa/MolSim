//
// Created by leo on 01.11.22.
//
#pragma once
#include <functional>
#include <vector>

#include "Particle.h"

class ParticleContainer {
 private:
  std::vector<Particle> vector;

 public:
  /**
   *
   * @param unaryFunction The function which gets applied to all particles in
   * the container
   */
  void forEach(std::function<void(Particle &)> &unaryFunction);
  /**
   *
   * @param binaryFunction The function that gets applied to each possible pair
   * of Particles
   */
  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction);

  std::vector<Particle> &getVectorRef();
};
