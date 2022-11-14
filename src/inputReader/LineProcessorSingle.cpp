//
// Created by leo on 12.11.22.
//

#include "LineProcessorSingle.h"
void LineProcessorSingle::processLine(std::istringstream &dataStream,
                                      ParticleContainer &container) {
  for (auto &xj : x) {
    dataStream >> xj;
  }
  for (auto &vj : v) {
    dataStream >> vj;
  }
  if (dataStream.eof()) {
    // TODO log here that file has wrong format
    exit(-1);
  }
  dataStream >> m;
  container.emplace_back(x, v, m);
}
