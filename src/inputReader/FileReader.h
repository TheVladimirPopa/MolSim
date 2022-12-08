/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once
#include <vector>

#include "ILineProcessor.h"
#include "LineProcessorCuboid.h"
#include "LineProcessorSingle.h"
#include "LineProcessorSphere.h"
#include "dataStructures/IContainer.h"
#include "dataStructures/Particle.h"

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  /**
   * Reads in a file with single particles and emplaces the read particles in
   * the ParticleContainer
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file, see single-format specified in
   * README.md
   */
  static void readFileSingle(IContainer &particles, char *filename) {
    LineProcessorSingle lineProcessor{};
    readFile(particles, filename, lineProcessor);
  }

  /**
   * Reads in a file with particles packed in cuboids and emplaces the read
   * particles in the ParticleContainer
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file, see cuboid-format specified in
   * README.md
   */
  static void readFileCuboid(IContainer &particles, char *filename) {
    LineProcessorCuboid lineProcessor{};
    readFile(particles, filename, lineProcessor);
  }

  /**
   * Reads in a file with particles packed in spheres and emplaces the read
   * particles in the ParticleContainer
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file, see cuboid-format specified in
   * README.md
   */
  static void readFileSphere(IContainer &particles, char *filename) {
    LineProcessorSphere lineProcessor{};
    readFile(particles, filename, lineProcessor);
  }

  /**
   * Reads in a file and invokes lineProcessor for each line, which emplaces
   * then the particles in the specified container
   * @param particles The particleContainer in which the particles get emplaced
   * @param filename Path to the input file
   * @param lineProcessor The lineprocessor which gets called for every line
   * which is not a comment and not the line with the number of particles
   * @note This method is only public for testing-reasons
   */
  static void readFile(IContainer &particles, char *filename,
                       ILineProcessor &lineProcessor);
};
