#include "NanoTubeStructure.h"

NanoTubeStructure::NanoTubeStructure(std::array<size_t, 3> dimensions, double stiffness, double bondLength,
                                     double cutOff, std::vector<Particle>& particles)
    : MembraneStructure(dimensions, stiffness, bondLength, cutOff, particles) {}

void NanoTubeStructure::applyForce(Particle& p1, Particle& p2) {
  size_t diff = p1.getId() < p2.getId() ? p2.getId() - p1.getId() : p1.getId() - p2.getId();

  if (diff == 1 || diff == dimensions[dimWidth]) return MembraneStructure::applyDirectForce(p1, p2);

  if (diff == dimensions[dimWidth] - 1 || diff == dimensions[dimWidth] + 1) return applyDiagonalForce(p1, p2);

  return applyRepulsiveForce(p1, p2);
}

