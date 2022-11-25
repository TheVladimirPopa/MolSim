/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <list>

#include "IWriter.h"
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "outputWriter/vtk-unstructured.h"

/**
 * This class implements the functionality to generate vtk output from
 * particles.
 */
class VTKWriter : public IWriter {
 public:
  VTKWriter();

  virtual ~VTKWriter();

  void writeFile(const std::string &filename, int iteration,
                 IContainer &particles) override;

 private:
  /**
   * Setup internal data structures and prepare to plot data
   * @param numParticles
   */
  void initializeOutput(int numParticles);

  /**
   * writes the final output file.   *
   * @param filename the base name of the file to be written.
   * @param iteration the number of the current iteration,
   *        which is used to generate an unique filename
   */
  void writeFile(const std::string &filename, int iteration);

  /**
   * plot type, mass, position, velocity and force of a particle.
   * @note: initializeOutput() must have been called before.
   * @param p The particle that gets plotted
   */
  void plotParticle(Particle &p);

  VTKFile_t *vtkFile;
};
