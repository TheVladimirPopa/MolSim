/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>

#include "IWriter.h"
#include "dataStructures/IContainer.h"
#include "dataStructures/Particle.h"

class XYZWriter : public IWriter {
 public:
  XYZWriter();

  virtual ~XYZWriter();

  void writeFile(const std::string &filename, int iteration,
                 IContainer &particles) override;
};
