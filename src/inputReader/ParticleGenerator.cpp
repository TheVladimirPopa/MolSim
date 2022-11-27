//
// Created by leo on 11.11.22.
//

#include "ParticleGenerator.h"

#include <array>
using std::array;
using vec3d = std::array<double, 3>;
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include "utils/MaxwellBoltzmannDistribution.h"

void ParticleGeneration::addCuboidToParticleContainer(
    IContainer &container, ParticleGeneration::cuboid const &data) {
  size_t dimension = (data.dimension[2] == 1 ? 2 : 3);
  constexpr double meanVel = 0.1;

  int numParticles = data.dimension[0] * data.dimension[1] * data.dimension[2];
  if (container.size() + numParticles > container.capacity()) {
    // Reserve so much that this cuboid would fit in twice to reduce
    // reallocations
    spdlog::debug("Resizing the particle container");
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

/**
 * Helper function for ParticleContainer::addSphereToParticleContainer.
 * Given the location of a particle it will place 4 particles at once in 2d or
 * 8 particles at once in 3d, if that location is within the sphere.
 * @param container Container that receives particles if location is in sphere
 * @param data The description of the sphere
 * @param delta Location offset for the newly added particles
 * @param dimension Dimension of the sphere
 * @param bolzmann_v The Bolzmann velocity
 */
void addParticlesIfInSphere(IContainer &container,
                            ParticleGeneration::sphere const &data, vec3d delta,
                            int dimension, double bolzmann_v) {
  auto is_in_sphere = [&data](vec3d pos) {
    return L2Norm(pos - data.position) <= (data.radius * data.distance);
  };

  // Iterate binary vector over all 4 or 8 possible configurations for signs
  bool checked = false;
  for (unsigned int sign_bin = 0b0000; sign_bin < 0b1000; sign_bin++) {
    if (dimension == 2 && ((sign_bin & 0b100) != 0)) continue;

    vec3d pos = data.position;

    vec3d signs = {((sign_bin & 0b001) == 0 ? 1.0 : -1.0),
                   ((sign_bin & 0b010) == 0 ? 1.0 : -1.0),
                   ((sign_bin & 0b100) == 0 ? 1.0 : -1.0)};

    pos = pos + signs * delta;

    if (!checked && !is_in_sphere(pos)) break;
    checked = true;

    vec3d bolz_v = maxwellBoltzmannDistributedVelocity(bolzmann_v, dimension);
    container.emplace_back(pos, bolz_v + data.velocity, data.mass, data.type);
  }
}

void ParticleGeneration::addSphereToParticleContainer(
    IContainer &container, ParticleGeneration::sphere const &data) {
  constexpr double meanVel = 0.1;

  size_t numParticles = data.radius * data.radius;
  numParticles =
      (data.dimension == 3 ? numParticles * data.radius : numParticles);

  if (container.size() + numParticles > container.capacity()) {
    spdlog::debug("Resizing the particle container");
    container.reserve(container.size() + (2 * numParticles));
  }

  // Algorithm: Consider a sphere placed perfectly in a cube. Slice the
  // sphere and cube into 8 (or 4 in 2d) smaller cubes by slicing along the
  // center of each surface. Iterate over particle locations in 1 of those small
  // cubes and add a particle whenever a location lies within the sphere. Mirror
  // every particle added in this way 8 times (or 4 times in 2d).

  for (unsigned int x = 1; x <= data.radius; ++x) {
    for (unsigned int y = 1; y <= data.radius; ++y) {
      for (unsigned int z = 1; z <= data.radius; ++z) {
        // delta describes offset of a particle from center point
        vec3d delta = {(x - 1) * data.distance + 0.5 * data.distance,
                       (y - 1) * data.distance + 0.5 * data.distance,
                       (z - 1) * data.distance + 0.5 * data.distance};
        if (data.dimension == 2) delta[2] = 0.0;

        addParticlesIfInSphere(container, data, delta, data.dimension, meanVel);
        if (data.dimension == 2) break;
      }
    }
  }

  spdlog::debug("Added sphere particles to particle container.");
}