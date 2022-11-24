#include "LineProcessorSphere.h"
#include "ParticleGenerator.h"

void LineProcessorSphere::processLine(std::istringstream &dataStream,
                                      ParticleContainer &container) {
  for (auto &xj : sphere.position) {
    dataStream >> xj;
  }

  for (auto &vj : sphere.velocity) {
    dataStream >> vj;
  }

  dataStream >> sphere.radius;

  dataStream >> sphere.dimension;

  dataStream >> sphere.distance;

  dataStream >> sphere.mass;

  dataStream >> sphere.type;


  ParticleGeneration::addSphereToParticleContainer(container, sphere);
}
