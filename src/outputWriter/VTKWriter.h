/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <list>

#include "IWriter.h"
#include "Particle.h"
#include "ParticleContainer.h"
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
                 ParticleContainer &particles) override;

 private:
  void initializeOutput(int numParticles);

  void writeFile(const std::string &filename, int iteration);

  void plotParticle(Particle &p);

  VTKFile_t *vtkFile;
};
