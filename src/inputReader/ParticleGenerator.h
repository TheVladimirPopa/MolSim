//
// Created by leo on 11.11.22.
//
#pragma once
#include <array>

#include "dataStructures/ParticleContainer.h"

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
  double distance = 1;
  /// The mass of a single particle
  double mass = 1;
  /// The type the particles have
  int type = 0;
};

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

};  // namespace ParticleGeneration
