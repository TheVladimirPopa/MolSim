/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once
#include <vector>

#include "ILineProcessor.h"
#include "LineProcessorSingle.h"
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  /**
   * Reads in a file with single particles and emplaces the read particles in
   * the ParticleContainer
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file
   */
  static void readFileSingle(ParticleContainer &particles, char *filename) {
    LineProcessorSingle lineProcessor{};
    readFile(particles, filename, lineProcessor);
  }

 private:
  /**
   * Reads in a file and invokes lineProcessor for each line, which emplaces
   * then the particles in the specified container
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file
   * @param lineProcessor The lineprocessor which gets called for every line
   * which is not a comment and not the line with the number of particles
   */
  static void readFile(ParticleContainer &particles, char *filename,
                       ILineProcessor &lineProcessor);
};
