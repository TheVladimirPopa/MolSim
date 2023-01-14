#pragma once

#include <vector>

#include "Particle.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

struct MembraneDefaultForce {
  void inline operator()(Particle& x, Particle& y) const {
    auto diff = x.getX() - y.getX();

    auto force = molecule.getStiffness() * (1 / L2Norm(diff)) * (L2Norm(diff) - molecule.getAverageBondLength()) * diff;

    x.applySymmetricForce(y, force);
  }
};

struct MembraneDiagonalForce {
  void inline operator()(Particle& x, Particle& y) const {
    constexpr double sqrt2 = 1.41421356237309504880168872;
    auto diff = x.getX() - y.getX();

    auto force =
        molecule.getStiffness() * (1 / L2Norm(diff)) * (L2Norm(diff) - sqrt2 * molecule.getAverageBondLength()) * diff;

    x.applySymmetricForce(y, force);
  }
};

class Molecule {
 private:
  std::vector<Particle> particles;

 public:
  // Proof of concept testcode for templates
  template <typename binaryFun1, typename binaryFun2>
  void applyInternalForces(binaryFun1 defaultForce, binaryFun2 diagonalForce) {
    size_t size = particles.size();
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = i + 1; j < size; ++j) {
        // todo: iterating over all pairs of particles makes no sense here.
        //  we need a custom particle type
        defaultForce(particles[i], particles[j]);
      }
    }
  };

  void test() {
    constexpr auto lambda = [](Particle& x, Particle& y) -> void { x.getSigma() + y.getSigma(); };

    // Test to see if this gets inlined without problems.
    applyInternalForces(MembraneDefaultForce{}, MembraneDiagonalForce{});
  }
};
