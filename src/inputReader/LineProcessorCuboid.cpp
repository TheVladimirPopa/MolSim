//
// Created by leo on 12.11.22.
//

#include "LineProcessorCuboid.h"

#include "ParticleGenerator.h"

void LineProcessorCuboid::processLine(std::istringstream &dataStream,
                                      IContainer &container) {
  for (auto &xj : cuboid.position) {
    dataStream >> xj;
  }

  for (auto &vj : cuboid.velocity) {
    dataStream >> vj;
  }

  for (auto &nj : cuboid.dimension) {
    dataStream >> nj;
  }
  dataStream >> cuboid.distance;

  dataStream >> cuboid.mass;

  dataStream >> cuboid.type;

  ParticleGeneration::addCuboidToParticleContainer(container, cuboid);
}
