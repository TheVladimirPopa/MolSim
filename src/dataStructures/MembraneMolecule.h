#pragma once

#include <vector>

#include "Particle.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
#include "model/HarmonicPotentialModel.h"
using ArrayUtils::L2Norm;

class MembraneMolecule {
 private:
  /// The dimensions of the membrane. Note: One dimensions MUST be 1 as membranes are single layered.
  const std::array<size_t, 3> dimensions{2, 2, 1};

  /// Defines where in the particles vector the first particle of the membrane is
  const size_t startIndex;

  /// Defines where in the particles vector the last particle of the membrane is
  const size_t particleCount;

  /// The vector that contains the particles of the membrane
  std::vector<Particle>* particles{nullptr};

  /// A particle and the force that gets applied to it in each iteration
  struct MembraneForce {
    /// The particle
    size_t particleId{0xffffffff};

    /// The force
    std::array<double, 3> force{0., 0., 0.};

    /// The number of times the force gets applied
    unsigned long timeSpan{0};
  };

  /// The forces that get applied to individually selected particles
  std::vector<MembraneForce> artificialForces;

  /// The stiffness factor of the membrane
  double const stiffness{};

  /// The average bond length within the membrane
  double const bondLength{};

  /// The index of the dimension that defines the height e.g. 0
  int dimHeight{};

  /// The index of the dimension that defines the width e.g. 1
  int dimWidth{};

  /// The model defining forces between particles
  HarmonicPotentialModel physics;

  // Helper functions to find particles
  /**
   * Takes a 3d grid position and checks whether the position is part of the membrane.
   * @param pos A 3d grid position, e.g. {42, 42 ,42} (not part of membrane as the membrane is single layered)
   * @return true: position is part of the membrane, false: position is not part of the membrane
   */
  [[nodiscard]] bool positionIsInMembrane(std::array<unsigned long, 3> pos) const;

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
  [[maybe_unused]] [[nodiscard]] Particle& getParticleByPosition(std::array<unsigned long, 3> pos);

  // todo: NEIGHBOURS ARE ACTUALLY UNUSED?
  /**
   * Once the particles are part of the membrane, this method assigns them their members.
   */
  // void linkParticles();

  /**
   * Applies the default force that is meant to be applied to direct neighbours
   * (= particles that are either horizontal or vertical neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void applyDirectForce(Particle& x, Particle& y) const;

  /**
   * Applies the diagonal force that is meant to be applied to diagonal neighbours
   * (= particles that are diagonal neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void applyDiagonalForce(Particle& x, Particle& y) const;

 public:
  MembraneMolecule(std::array<size_t, 3> dimensions, double stiffness, double bondLength,
                   std::vector<Particle>& particles);

  /**
   * Defines the force that gets applied to a single, specified particle of the membrane in each iteration.
   * @param row The row of the particle within the membrane
   * @param column The column of the particle within the membrane
   * @param force The force that gets applied to the particle
   * @param timeSpan The number of iteration that force gets applied
   */
  void addForceToParticle(unsigned int row, unsigned int column, std::array<double, 3> force, unsigned long timeSpan);

  /**
   * For each particle applies the membrane forces to direct (horizontal and vertical) neighbours
   * and diagonal neighbours.
   */
  void applyInternalForces();

  /**
   * Applies the artificial forces that were defined in the xml file.
   */
  void applyArtificialForces();

  /**
   * Repulses the particles p1 and p2 if they are close enough.
   * @param p1 The first particle which repulses the second
   * @param p2 The second particle which repulses the first
   */
  void applyRepulsiveForce(Particle& p1, Particle& p2);

  /**
   * Applies the correct inner molecule force selected based on indices.
   * @param p1 Particle 1
   * @param p2 Particle 2
   * @param indexP1 Index of particle 1
   * @param indexP2 Index of particle 2
   */
  void applyForce(Particle& p1, Particle& p2, size_t indexP1, size_t indexP2);

  /**
   * Returns whether the two passed particle indices are neighbours (direct or diagonal)
   * @param indexA Index of first particle
   * @param indexB Index of second particle
   * @return Whether first and second particle are neighbours
   */
  [[maybe_unused]] bool indicesAreNeighbours(size_t indexA, size_t indexB);

  bool hasArtificalForces() { return !artificialForces.empty(); }

  void setMoleculeVectorIndex(size_t index);

  auto begin() { return particles->begin(); };

  auto end() {
    auto it = particles->begin();
    std::advance(it, particleCount);
    return it;
  }

  [[nodiscard]] size_t size() const { return particleCount; }
};