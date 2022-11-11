//
// Created by leo on 02.11.22.
//
#pragma once
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
class IWriter {
 public:
  /**
   * Write the current state of each particle in the container to the specified
   * file
   * @param filename Path to output file (version numbers and filetypes are
   * added automatically)
   * @param iteration Current iteration that gets used to generate a unique
   * filename
   * @param particles The particle container with all particles
   */
  virtual void writeFile(const std::string &filename, int iteration,
                         ParticleContainer &particles) = 0;
};