#pragma once

#include <vector>

#include "MembraneParticle.h"
#include "Particle.h"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

class MembraneMolecule {
 private:
  std::vector<MembraneParticle> particles;
  double const stiffness{};
  double const bondLength{};
  std::array<size_t, 3> dimensions{2, 2, 1};
  int dimWidth{};
  int dimHeight{};

  bool positionIsInMembrane(std::array<unsigned long, 3> pos);
  size_t positionToIndex(std::array<unsigned long, 3> pos);
  MembraneParticle& getParticleByPosition(std::array<unsigned long, 3> pos);
  void linkMembraneParticles();

  void inline applyDefaultForce(MembraneParticle& x, MembraneParticle& y) const {
    auto diff = x.getX() - y.getX();

    auto force = stiffness * (1 / L2Norm(diff)) * (L2Norm(diff) - bondLength) * diff;

    x.applySymmetricForce(y, force);
  }

  void inline applyDiagonalForce(MembraneParticle& x, MembraneParticle& y) const {
    constexpr double sqrt2 = 1.41421356237309504880168872;
    auto diff = x.getX() - y.getX();

    auto force = stiffness * (1 / L2Norm(diff)) * (L2Norm(diff) - sqrt2 * bondLength) * diff;

    x.applySymmetricForce(y, force);
  }

 public:
  MembraneMolecule(double stiffness, double bondLength, std::vector<MembraneParticle>&& particles)
      : particles{particles}, stiffness{stiffness}, bondLength{bondLength} {
    // The membrane consists out of a single layer. Find out indices of which dimensions are relevant.
    for (int i = 0; i < 3; i++) {
      if (dimensions[i] == 1) {
        dimHeight = (i + 1) % 3;
        dimWidth = (i + 2) % 3;
        break;
      }
    }

    if (dimHeight > dimWidth) std::swap(dimHeight, dimWidth);

    // Check to avoid time-consuming edge case handling when applying internal forces
    if (particles.size() < 4 || dimensions[dimHeight] < 2 || dimensions[dimWidth] < 2) {
      spdlog::error("Membranes must at least have 2x2 dimensions");
      exit(EXIT_FAILURE);
    }

    linkMembraneParticles();
  }

  void applyInternalForces() {
    //  o o o o   <--- We iterate over the membrane particle grid.
    //  o o o o        Direct forces: For each particle, we build a pair, by taking the right and lower particle.
    //  o o o o        Diagonal forces: For each particle, we build a pair, by taking the left and right lower particle.
    const unsigned int width = dimensions[dimWidth];

    size_t size = particles.size();
    for (size_t i = 0; i < size; i++) {
      // Right neighbour
      if ((i % width) + 1 < width) applyDefaultForce(particles[i], particles[i + 1]);

      // Lower neighbour
      if (i + width < size) applyDefaultForce(particles[i], particles[i + width]);

      // Left lower diagonal neighbour
      if (i + width - 1 < size) applyDiagonalForce(particles[i], particles[i + width - 1]);

      // Right lower diagonal neighbour
      if (i + width + 1 < size) applyDiagonalForce(particles[i], particles[i + width + 1]);
    }
  };

  std::vector<MembraneParticle>& getParticles() { return particles; }
};
