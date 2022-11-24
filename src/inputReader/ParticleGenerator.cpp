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
    ParticleContainer &container, ParticleGeneration::cuboid const &data) {
  constexpr size_t dimension = 2;  // Todo: This probably shouldn't be constexpr
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

void addParticlesIfInSphere(ParticleContainer &container,
                            ParticleGeneration::sphere const &data, vec3d delta,
                            int dimension, double bolzmann_v) {
  bool checked = false;
  auto is_in_sphere = [&data, &checked](vec3d pos) {
    // check once per function call to avoid asymmetries due to rounding errors
    if (checked) return checked;
    checked = true;

    pos = pos - data.position;

    // todo: this might fail once we start rounding. an alternative would be to
    //   first place outer molecules and calculate cutoff distance based on those
    return L2Norm(pos) <= (data.radius * data.distance);
  };

  for (unsigned int sign_bin = 0b0000; sign_bin < 0b1000; sign_bin++) {
    vec3d pos = data.position;

    vec3d sign = {((sign_bin & 0b001) == 0 ? 1.0 : -1.0),
                  ((sign_bin & 0b010) == 0 ? 1.0 : -1.0),
                  ((sign_bin & 0b100) == 0 ? 1.0 : -1.0)};

    pos = pos + sign * delta;

    if (!is_in_sphere(pos)) break;

    vec3d bolz_v = maxwellBoltzmannDistributedVelocity(bolzmann_v, dimension);
    container.emplace_back(pos, bolz_v + data.velocity, data.mass, data.type);
  }
}

void ParticleGeneration::addSphereToParticleContainer(
    ParticleContainer &container, ParticleGeneration::sphere const &data) {
  size_t dimension = (data.radius == 0 ? 2 : 3);
  constexpr double meanVel = 0.1;  // todo rausziehen

  // todo: correct number
  size_t numParticles = data.radius * data.radius;
  numParticles = (dimension == 3 ? numParticles * data.radius : numParticles);

  if (container.size() + numParticles > container.capacity()) {
    // Reserve so much that this cuboid would fit in twice to reduce
    // reallocations
    spdlog::debug("Resizing the particle container");
    container.reserve(container.size() + (2 * numParticles));
  }

  // Imagine a cube in 3d space with a point at its center.
  // We slice the big cube apart into 8 equally sized smaller cubes.
  // Our algorithm only looks at 1 of these 8 cubes. We want to generate part of
  // the sphere. We divide that cube again, into smaller 1 molecule sized cubes.
  // We iterate over these mini cubes. We add all minicubes to our particle
  // container, whose distance to the center is smaller than the radius.
  // We mirror along each dimension once and get the final sphere.

  for (unsigned int x = 1; x <= data.radius; ++x) {
    for (unsigned int y = 1; y <= data.radius; ++y) {
      for (unsigned int z = 1; z <= data.radius; ++z) {
        if (dimension == 2) z = 0;

        vec3d delta = {(x - 1) * data.distance + 0.5 * data.distance,
                       (y - 1) * data.distance + 0.5 * data.distance,
                       (z - 1) * data.distance + 0.5 * data.distance};

        addParticlesIfInSphere(container, data, delta, dimension, meanVel);

        if (dimension == 2) break;
      }
    }
  }
}