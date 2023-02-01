//
// Created by leo on 11.11.22.
//

#include "ParticleGenerator.h"

#include <array>
using std::array;
using vec3d = std::array<double, 3>;
#include "dataStructures/MembraneStructure.h"
#include "dataStructures/NanoTubeStructure.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include <cmath>
#define M_PIl 3.141592653589793238462643383279502884L
using ParticleGeneration::Axis;

void ParticleGeneration::addCuboidToParticleContainer(IContainer &container, ParticleGeneration::cuboid const &data) {
  int numParticles = data.dimension[0] * data.dimension[1] * data.dimension[2];
  if (container.size() + numParticles > container.capacity()) {
    // Reserve so much that this cuboid would fit in twice to reduce
    // reallocations
    spdlog::debug("Resizing the particle container");
    container.reserve(container.size() + (2 * numParticles));
  }

  std::array<double, 3> position{};
  for (int x = 0; x < data.dimension[0]; ++x) {
    for (int y = 0; y < data.dimension[1]; ++y) {
      for (int z = 0; z < data.dimension[2]; ++z) {
        position = data.position;
        position[0] += x * data.distance;
        position[1] += y * data.distance;
        position[2] += z * data.distance;
        container.emplace_back(position, data.velocity, data.mass, data.type);
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
void addParticlesIfInSphere(IContainer &container, ParticleGeneration::sphere const &data, vec3d delta, int dimension) {
  auto is_in_sphere = [&data](vec3d pos) { return L2Norm(pos - data.position) <= (data.radius * data.distance); };

  // Iterate binary vector over all 4 or 8 possible configurations for signs
  bool checked = false;
  for (unsigned int sign_bin = 0b0000; sign_bin < 0b1000; sign_bin++) {
    if (dimension == 2 && ((sign_bin & 0b100) != 0)) continue;

    vec3d pos = data.position;

    vec3d signs = {((sign_bin & 0b001) == 0 ? 1.0 : -1.0), ((sign_bin & 0b010) == 0 ? 1.0 : -1.0),
                   ((sign_bin & 0b100) == 0 ? 1.0 : -1.0)};

    pos = pos + signs * delta;

    if (!checked && !is_in_sphere(pos)) break;
    checked = true;
    container.emplace_back(pos, data.velocity, data.mass, data.type);
  }
}

void ParticleGeneration::addSphereToParticleContainer(IContainer &container, ParticleGeneration::sphere const &data) {
  size_t numParticles = data.radius * data.radius;
  numParticles = (data.dimension == 3 ? numParticles * data.radius : numParticles);

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
        vec3d delta = {(x - 1) * data.distance + 0.5 * data.distance, (y - 1) * data.distance + 0.5 * data.distance,
                       (z - 1) * data.distance + 0.5 * data.distance};
        if (data.dimension == 2) delta[2] = 0.0;

        addParticlesIfInSphere(container, data, delta, data.dimension);
        if (data.dimension == 2) break;
      }
    }
  }

  spdlog::debug("Added sphere particles to particle container.");
}

void ParticleGeneration::addMembraneToParticleContainer(IContainer &container,
                                                        ParticleGeneration::membrane const &data) {
  if (data.dimension[0] != 1 && data.dimension[1] != 1 && data.dimension[2] != 1) {
    spdlog::error("Only single layer membranes are supported. Please check your input file.");
    exit(EXIT_FAILURE);
  }

  // Insert particles into container
  std::array<double, 3> position{};
  for (int x = 0; x < data.dimension[0]; ++x) {
    for (int y = 0; y < data.dimension[1]; ++y) {
      for (int z = 0; z < data.dimension[2]; ++z) {
        position = data.position;
        position[0] += x * data.distance;
        position[1] += y * data.distance;
        position[2] += z * data.distance;

        container.emplace_back(position, data.velocity, data.mass, data.type);
      }
    }
  }

  // Then generate membrane structure. The last particles of the container are consider to be part of the structure.
  MembraneStructure membrane{toSizeTArray(data.dimension), data.stiffness, data.bondLength, data.cutOffRadius,
                             container.getParticlesRef()};
  for (auto &mf : data.membraneForces)
    membrane.addForceToParticle(mf.row, mf.column, std::array<double, 3>{mf.x, mf.y, mf.z}, mf.timeSpan);

  container.push_back(membrane);
}

std::vector<v3d> ParticleGeneration::getCircleLocations(v3d position, double radius, double count, Axis axis) {
  size_t x_index, y_index;
  if (axis == Axis::X) {
    x_index = 1;
    y_index = 2;
  } else if (axis == Axis::Y) {
    x_index = 0;
    y_index = 2;
  } else {
    x_index = 0;
    y_index = 1;
  }

  double degree_per_part = 360. / count;
  std::pair<double, double> polar_coord{radius, 0};

  std::vector<v3d> locations;
  for (int i = 0; i < count; i++, polar_coord.second += degree_per_part) {
    std::pair<double, double> cart_coord{radius * std::cos(polar_coord.second * M_PIl / 180),
                                         radius * std::sin(polar_coord.second * M_PIl / 180)};

    auto particle_pos = position;
    particle_pos[x_index] += cart_coord.first;
    particle_pos[y_index] += cart_coord.second;
    locations.push_back(particle_pos);
  }
  return locations;
}

void ParticleGeneration::addNanoTubeToParticleContainer(IContainer &container,
                                                        ParticleGeneration::nanotube const &data) {
  if (data.dimension[2] != 1) {
    spdlog::error("For a nanotube the z dimension must be 1.");
    exit(EXIT_FAILURE);
  };

  // Build up nanotube ring by ring
  auto center = data.position;  // The center of the circle that gets drawn
  auto &center_height = center[static_cast<size_t>(data.axis)];
  int height = data.dimension[0];  // Number of circles lying on each other

  // Draw circle, then increase height by data distance, then draw circle, and so on until height is reached
  for (int currentHeight = 0; currentHeight < height; ++currentHeight, center_height += data.distance) {
    for (auto loc : getCircleLocations(center, data.radius, data.dimension[1], data.axis)) {
      container.emplace_back(loc, data.velocity, data.mass, data.type);
    }
  }

  NanoTubeStructure nanoTube{toSizeTArray(data.dimension), data.stiffness, data.bondLength, data.cutOffRadius,
                             container.getParticlesRef()};

  for (auto &mf : data.membraneForces)
    nanoTube.addForceToParticle(mf.row, mf.column, std::array<double, 3>{mf.x, mf.y, mf.z}, mf.timeSpan);

  container.push_back(nanoTube);
}