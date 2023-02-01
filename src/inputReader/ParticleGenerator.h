//
// Created by leo on 11.11.22.
//
#pragma once
#include <array>

#include "dataStructures/IContainer.h"
#include "dataStructures/LinkedCellsContainer.h"

namespace ParticleGeneration {
/**
 * Struct containing all the values specifying a cuboid
 */
struct cuboid {
  /// The position of the left lower corner of the cuboid
  std::array<double, 3> position{0., 0., 0.};
  /// What dimension in particle count the cuboid has
  std::array<int, 3> dimension{1, 1, 1};
  /// The velocity of the particles, without Brownian Motion
  std::array<double, 3> velocity{0., 0., 0.};
  /// The distance between adjacent particles(mesh width)
  double distance{1.0};
  /// The mass of a single particle
  double mass{1.0};
  /// The type the particles have
  int type{0};
};

/**
 * Struct containing all the values specifying a sphere
 */
struct sphere {
  /// The position of the left lower corner of the sphere
  std::array<double, 3> position{0., 0., 0.};
  /// What radius in particle count the sphere has
  unsigned int radius{1};
  /// What dimension the sphere has (2d or 3d)
  unsigned int dimension{2};
  /// The velocity of the particles, without Brownian Motion
  std::array<double, 3> velocity{0., 0., 0.};
  /// The distance between adjacent particles(mesh width)
  double distance{1.0};
  /// The mass of a single particle
  double mass{1.0};
  /// The type the particles have
  int type{0};
};

/**
 * The force that applies to a specific membrane particle for timeSpan iterations.
 */
struct MembraneForce {
  unsigned int row{};
  unsigned int column{};
  double x{};
  double y{};
  double z{};
  unsigned long timeSpan{};
};

/**
 * Struct containing all the values specifying a membrane. In essence a membrane structure has the structure of a
 * cuboid where the size of 1 dimension must be 1. Beware that the force calculation is different to a cuboid or sphere.
 * Structures get special treatment with respect to the force calculation, to ensure e.g. that they do not accidentally
 * get ripped apart.
 */
struct membrane : cuboid {
  double bondLength{2.2};
  double stiffness{300.};
  double cutOffRadius{1e30};
  std::vector<MembraneForce> membraneForces;
};

/**
 * Struct containing all the values specifying a nanotube.
 */
enum Axis { X = 0, Y = 1, Z = 2 };

struct nanotube : membrane {
  double radius;
  Axis axis{Axis::X};

  std::vector<MembraneForce> membraneForces;
};

/**
 * Convert array to size_t array
 * @tparam NUMTYPE A type that can be casted to size_t
 * @param inArray The array of size 3
 * @return The casted std::array<size_t, 3>
 */
template <typename NUMTYPE>
std::array<size_t, 3> toSizeTArray(std::array<NUMTYPE, 3> inArray) {
  return {static_cast<size_t>(inArray[0]), static_cast<size_t>(inArray[1]), static_cast<size_t>(inArray[2])};
}
/**
 * Generate all the single particles for a cuboid and places them in the
 * container
 * @note The particle velocity is calculated with @see
 * MaxwellBoltzmannDistribution.h
 * @param container The container in which all the particles get placed. This
 * method will reserve additional place, if it is to small.
 * @param data The cuboid struct which holds the data for the generation
 */
void addCuboidToParticleContainer(IContainer &container, cuboid const &data);

/**
 * Generate all the single particles for a sphere and places them in the
 * container
 * @note The particle velocity is calculated with @see
 * MaxwellBoltzmannDistribution.h
 * @param container The container in which all the particles get placed. This
 * method will reserve additional place, if it is to small.
 * @param data The sphere struct which holds the data for the generation
 */
void addSphereToParticleContainer(IContainer &container, sphere const &data);

/**
 * Adds a membrane structure to the container. Structures are treated differently to particles in the force calculation.
 * @param container The container that receives the membrane structure
 * @param data The membrane struct which holds the data for the generation
 */
void addMembraneToParticleContainer(IContainer &container, membrane const &data);

/**
 * Adds a nanotube structure to the container. Structures are treated differently to particles in the force calculation.
 * @param container The container that receives the membrane structure
 * @param data The membrane struct which holds the data for the generation
 */
void addNanoTubeToParticleContainer(IContainer &container, nanotube const &data);

/**
 * Get ordered locations of outline points of a circle
 * @param position center of the circle
 * @param radius Radius of the circle
 * @param count Number of outline points
 * @param axis The normal of the circle. / The Dimension that is orthogonal to the circle.
 * @return The positions
 */
std::vector<v3d> getCircleLocations(v3d position, double radius, double count, Axis axis);
};  // namespace ParticleGeneration
