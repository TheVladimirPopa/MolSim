/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>

#include "IWriter.h"
#include "Particle.h"
#include "ParticleContainer.h"

namespace outputWriter {

class XYZWriter : public IWriter {
 private:
  std::ofstream file;

 public:
  XYZWriter();

  virtual ~XYZWriter();

  void writeFile(const std::string &filename, int iteration,
                 ParticleContainer &particles) override;
};

}  // namespace outputWriter
