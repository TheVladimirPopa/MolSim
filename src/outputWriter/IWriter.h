//
// Created by leo on 02.11.22.
//
#pragma once
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
class IWriter {
 public:
  virtual void writeFile(const std::string &filename, int iteration,
                         ParticleContainer &particles) = 0;
};