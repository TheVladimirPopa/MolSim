//
// Created by leo on 12.11.22.
//
#pragma once
#include <sstream>

#include "dataStructures/ParticleContainer.h"

class ILineProcessor {
 public:
  /**
   * Takes the dataStream of one line and places the specified particle(s)
   * into the container
   * @param dataStream The dataStream of one line, which is not a comment and
   * not the line specifying the number of entries
   * @param container The container where all the particles get placed in
   */
  virtual void processLine(std::istringstream &dataStream,
                           ParticleContainer &container) = 0;
};
