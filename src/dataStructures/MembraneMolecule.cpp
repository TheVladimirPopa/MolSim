#include "MembraneMolecule.h"

#include <iostream>

/*
void MembraneMolecule::linkMembraneParticles() {
  for (auto& particle : particles) {
    // Link direct neighbours
    for (int i = 0; i < 3; i++) {
      for (int offset : {-1, +1}) {
        auto pos = particle.getMembranePosition();
        pos[i] += offset;

        if (positionIsInMembrane(pos)) particle.addDirectNeighbourUnique(&getParticleByPosition(pos));
      }
    }

    // Link diagonal neighbours
    std::vector<std::array<unsigned long, 3>> diagonalOffsets;

    for (int offset1 : {-1, +1}) {
      for (int i = 0; i < 3; i++) {
        for (int offset2 : {-1, +1}) {
          for (int j = i + 1; j < 3; j++) {
            std::array<unsigned long, 3> offset{0, 0, 0};
            offset[i] = offset1;
            offset[j] = offset2;

            diagonalOffsets.push_back(offset);
          }
        }
      }
    }

    for (auto offset : diagonalOffsets) {
      auto pos = particle.getMembranePosition() + offset;
      if (positionIsInMembrane(pos)) particle.addDiagonalNeighbourUnique(&getParticleByPosition(pos));
    }
  }
}*/

bool MembraneMolecule::positionIsInMembrane(std::array<unsigned long, 3> pos) const {
  // unsigned long will underflow for negative values
  if (pos[0] >= dimensions[0] || pos[1] >= dimensions[1] || pos[2] >= dimensions[2]) return false;
  return true;
}

size_t MembraneMolecule::positionToIndex(std::array<unsigned long, 3> pos) const {
  return startIndex + pos[0] + (pos[1] * dimensions[0]) + (pos[2] * dimensions[0] * dimensions[1]);
}

[[maybe_unused]] Particle& MembraneMolecule::getParticleByPosition(std::array<unsigned long, 3> pos) {
  return particles->at(positionToIndex(pos));
}

void MembraneMolecule::applyDefaultForce(Particle& x, Particle& y) const {
  auto diff = y.getX() - x.getX();

  auto dist = L2Norm(diff);
  auto force = stiffness * (1 / dist) * (dist - bondLength) * diff;

  x.applySymmetricForce(y, force);
}

void MembraneMolecule::applyDiagonalForce(Particle& x, Particle& y) const {
  constexpr double sqrt2 = 1.41421356237309504880168872;
  auto diff = y.getX() - x.getX();

  auto dist = L2Norm(diff);
  auto force = stiffness * (1 / dist) * (dist - sqrt2 * bondLength) * diff;

  x.applySymmetricForce(y, force);
}

MembraneMolecule::MembraneMolecule(std::array<size_t, 3> dimensions, double stiffness, double bondLength,
                                   std::vector<Particle>& particles)
    : dimensions{dimensions},
      startIndex{particles.size() - (dimensions[0] * dimensions[1] * dimensions[2])},
      particleCount{dimensions[0] * dimensions[1] * dimensions[2]},
      particles{&particles},
      stiffness{stiffness},
      bondLength{bondLength} {
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

  // TODO: Put back?
  // linkMembraneParticles();
}

void MembraneMolecule::addForceToParticle(unsigned int row, unsigned int column, std::array<double, 3> force,
                                          unsigned long timeSpan) {
  MembraneForce mf{};

  std::array<unsigned long, 3> pos3d{0, 0, 0};
  pos3d[dimHeight] = row;
  pos3d[dimWidth] = column;
  if (!positionIsInMembrane(pos3d)) throw std::runtime_error("Membrane particle position for force is invalid.");

  mf.particleId = positionToIndex(pos3d);
  mf.force = force;
  mf.timeSpan = timeSpan;

  singleForces.push_back(mf);
}

void MembraneMolecule::applyInternalForces() {
  //  o o o o   <--- We iterate over the membrane particle grid.
  //  o o o o        Direct forces: For each particle, we build a pair, by taking the right and lower particle.
  //  o o o o        Diagonal forces: For each particle, we build a pair, by taking the left and right lower particle.
  const unsigned int width = dimensions[dimWidth];

  auto& particle = *particles;
  for (size_t i = startIndex; i < startIndex + particleCount; i++) {
    // Right neighbour
    if ((i % width) + 1 < width) applyDefaultForce(particle[i], particle[i + 1]);

    // Lower neighbour
    if (i + width < particleCount) applyDefaultForce(particle[i], particle[i + width]);

    // Left lower diagonal neighbour
    if (i % width != 0 && i + width - 1 < particleCount) applyDiagonalForce(particle[i], particle[i + width - 1]);

    // Right lower diagonal neighbour
    if ((i + 1) % width != 0 && i + width + 1 < particleCount)
      applyDiagonalForce(particle[i], particle[i + width + 1]);
  }

  // Apply forces defined for the individual particles within the membrane
  if (singleForces.empty()) return;

  bool allForcesUsedUp{true};
  for (auto& mf : singleForces) {
    if (mf.timeSpan <= 0) continue;

    particle[mf.particleId].applyForce(mf.force);
    mf.timeSpan--;
    allForcesUsedUp = false;
  }
  if (allForcesUsedUp) singleForces.clear();
}
