#include "MembraneParticle.h"

MembraneParticle::MembraneParticle(std::array<size_t, 3> membranePosition, std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type)
    : Particle(x_arg, v_arg, m_arg, type), membranePosition{membranePosition} {}

void MembraneParticle::addDirectNeighbourUnique(MembraneParticle* particle) {
  if (std::find(directNeighbours.begin(), directNeighbours.end(), particle) == directNeighbours.end()) {
    directNeighbours.push_back(particle);
    std::sort(directNeighbours.begin(), directNeighbours.end());
  }
}

void MembraneParticle::addDiagonalNeighbourUnique(MembraneParticle* particle) {
  if (std::find(diagonalNeighbours.begin(), diagonalNeighbours.end(), particle) == diagonalNeighbours.end()) {
    diagonalNeighbours.push_back(particle);
    std::sort(diagonalNeighbours.begin(), diagonalNeighbours.end());
  }
}
