//
// Created by leo on 11.11.22.
//

#include "ParticleGenerator.h"

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void ParticleGeneration::addCuboidToParticleContainer(
    ParticleContainer &container, ParticleGeneration::cuboid const &data) {
  constexpr size_t dimension = 2;
  constexpr double meanVel = 0.1;

  int numParticles = data.dimension[0] * data.dimension[1] * data.dimension[2];
  if (container.size() + numParticles > container.capacity()) {
    // Reserve so much that this cuboid would fit in twice to reduce
    // reallocations
    // TODO DEBUG log expand particle container
    container.reserve(container.size() + (2 * numParticles));
  }

  std::array<double, 3> position{};
  std::array<double, 3> velocity{};
  for (int x = 0; x < data.dimension[0]; ++x) {
    for (int y = 0; y < data.dimension[1]; ++y) {
      for (int z = 0; z < data.dimension[2]; ++z) {
        position = data.position;
        position[0] += x * data.distance;
        position[1] += y * data.distance;
        position[2] += z * data.distance;
        velocity = maxwellBoltzmannDistributedVelocity(meanVel, dimension);
        container.emplace_back(position, velocity + data.velocity, data.mass,
                               data.type);
      }
    }
  }
}