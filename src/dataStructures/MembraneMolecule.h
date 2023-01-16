#pragma once

#include <vector>

#include "MembraneParticle.h"
#include "Particle.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

class MembraneMolecule {
 private:
  /// The particles that are part of the molecule.
  std::vector<MembraneParticle> particles;

  /// The stiffness factor of the membrane
  double const stiffness{};

  /// The average bond length within the membrane
  double const bondLength{};

  /// The dimensions of the membrane. Note: One dimensions MUST be 1 as membranes are single layered.
  std::array<size_t, 3> dimensions{2, 2, 1};

  /// The index of the dimension that defines the height e.g. 0
  int dimHeight{};

  /// The index of the dimension that defines the width e.g. 1
  int dimWidth{};

  // Helper functions to find particles
  /**
   * Takes a 3d grid position and checks whether the position is part of the membrane.
   * @param pos A 3d grid position, e.g. {42, 42 ,42} (not part of membrane as the membrane is single layered)
   * @return true: position is part of the membrane, false: position is not part of the membrane
   */
  bool positionIsInMembrane(std::array<unsigned long, 3> pos) const;

  /**
   * Converts a 3d grid position to a vector index of the internal particles vector.
   * Note: For most cases getParticleByPosition is the better choice.
   * @param pos A 3d grid position of a particle in the membrane grid, e.g. {1,1,1}
   * @return The vector index corresponding to the position, e.g. {1, 1, 1} will return 1.
   */
  [[nodiscard]] size_t positionToIndex(std::array<unsigned long, 3> pos) const;

  /**
   * Returns the particle that holds the given 3d position.
   * Note: Check with positionIsInMembrane first, whether the location is valid.
   * @param pos Position where a particle sits
   * @return The particle
   */
  [[nodiscard]] MembraneParticle& getParticleByPosition(std::array<unsigned long, 3> pos);

  /**
   * Once the particles are part of the membrane, this method assigns them their members.
   */
  void linkMembraneParticles();

  /**
   * Applies the default force that is meant to be applied to direct neighbours
   * (= particles that are either horizontal or vertical neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void applyDefaultForce(MembraneParticle& x, MembraneParticle& y) const;

  /**
   * Applies the diagonal force that is meant to be applied to diagonal neighbours
   * (= particles that are diagonal neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void applyDiagonalForce(MembraneParticle& x, MembraneParticle& y) const;

 public:
  MembraneMolecule(double stiffness, double bondLength, std::vector<MembraneParticle>&& particles);

  /**
   * For each particle applies the membrane forces to direct (horizontal and vertical) neighbours
   * and diagonal neighbours.
   */
  void applyInternalForces();

  auto begin() { return particles.begin(); };

  auto end() { return particles.end(); }

  size_t size() { return particles.size(); }
};