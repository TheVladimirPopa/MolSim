#pragma once

#include <vector>

#include "MembraneParticle.h"
#include "Particle.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

// (This is a proof of concept for templating models.)
struct MembraneDefaultForce {
  void inline operator()(MembraneParticle& x, MembraneParticle& y, double stiffness, double bondLength) const {
    auto diff = x.getX() - y.getX();

    auto force = stiffness * (1 / L2Norm(diff)) * (L2Norm(diff) - bondLength) * diff;

    x.applySymmetricForce(y, force);
  }
};

struct MembraneDiagonalForce {
  void inline operator()(MembraneParticle& x, MembraneParticle& y, double stiffness, double bondLength) const {
    constexpr double sqrt2 = 1.41421356237309504880168872;
    auto diff = x.getX() - y.getX();

    auto force = stiffness * (1 / L2Norm(diff)) * (L2Norm(diff) - sqrt2 * bondLength) * diff;

    x.applySymmetricForce(y, force);
  }
};

// Note: If this would get extended further, this class should be abstracted into a Molecule class. As worksheet 5
//  marks the end of the project, this class suffices.
class Molecule {};
class MembraneMolecule : public Molecule {
 private:
  std::vector<MembraneParticle> particles;
  double const stiffness{};
  double const bondLength{};

  void linkNeighbours();

 public:
  MembraneMolecule(double stiffness, double bondLength, std::vector<MembraneParticle>&& particles)
      : stiffness{stiffness}, bondLength{bondLength}, particles{particles} {
    linkNeighbours();
  }

  template <typename directForceFunction, typename diagonalForceFunction>
  void applyInternalForces(directForceFunction directF, diagonalForceFunction diagonalForce) {
    size_t size = particles.size();
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = i + 1; j < size; ++j) {
        // todo: iterating over all pairs of particles makes no sense here.
        //  we need a custom particle type
        directF(particles[i], particles[j], stiffness, bondLength);
      }
    }
  };

  void test() {
    constexpr auto directF = MembraneDefaultForce{};
    constexpr auto diagonalF = MembraneDiagonalForce{};

    // Test to see if this gets inlined without problems.
    applyInternalForces(directF, diagonalF);
  }
};
