//
// Created by leo on 02.11.22.
//
#pragma once
#include "../Particle.h"
#include "ParticleContainer.h"
class OutputModel {
 public:
  virtual void writeFile(const std::string &filename, int iteration,
                         ParticleContainer &particles) = 0;
};