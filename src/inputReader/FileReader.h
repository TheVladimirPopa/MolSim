/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once
#include <vector>

#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  /**
   * Reads in a file and emplaces the read particle in the ParticleContainer
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file
   */
  static void readFile(ParticleContainer &particles, char *filename);
};
