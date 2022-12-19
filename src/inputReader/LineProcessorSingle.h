//
// Created by leo on 12.11.22.
//
#pragma once
#include "ILineProcessor.h"

class LineProcessorSingle : public ILineProcessor {
 public:
  void processLine(std::istringstream &dataStream, IContainer &container) override;

 private:
  /**
   * The array for the positions
   */
  std::array<double, 3> x{0., 0., 0.};
  /**
   * The array for the velocities
   */
  std::array<double, 3> v{0., 0., 0.};
  /**
   * Specifies the mass of the particle
   */
  double m = 0.;
};
