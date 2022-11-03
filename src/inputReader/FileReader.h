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

  static void readFile(ParticleContainer &particles, char *filename);
};
