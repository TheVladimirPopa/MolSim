#include "MembraneMolecule.h"

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

void MembraneMolecule::applyDirectForce(Particle& x, Particle& y) const { physics.addDirectForce(x, y); }

void MembraneMolecule::applyDiagonalForce(Particle& x, Particle& y) const { physics.addDiagonalForce(x, y); }

void MembraneMolecule::applyRepulsiveForce(Particle& p1, Particle& p2) { physics.addForces(p1, p2); }

MembraneMolecule::MembraneMolecule(std::array<size_t, 3> dimensions, double stiffness, double bondLength,
                                   std::vector<Particle>& particles)
    : dimensions{dimensions},
      startIndex{particles.size() - (dimensions[0] * dimensions[1] * dimensions[2])},
      particleCount{dimensions[0] * dimensions[1] * dimensions[2]},
      particles{&particles},
      stiffness{stiffness},
      bondLength{bondLength},
      physics(stiffness, bondLength) {
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

  artificialForces.push_back(mf);
}

void MembraneMolecule::applyInternalForces() {
  //  o o o o   <--- We iterate over the membrane particle grid.
  //  o o o o        Direct forces: For each particle, we build a pair, by taking the right and lower particle.
  //  o o o o        Diagonal forces: For each particle, we build a pair, by taking the left and right lower particle.
  const unsigned int width = dimensions[dimWidth];

  auto& particle = *particles;
  for (size_t i = startIndex; i < startIndex + particleCount; i++) {
    // Right neighbour
    if ((i % width) + 1 < width) applyDirectForce(particle[i], particle[i + 1]);

    // Lower neighbour
    if (i + width < particleCount) applyDirectForce(particle[i], particle[i + width]);

    // Left lower diagonal neighbour
    if (i % width != 0 && i + width - 1 < particleCount) applyDiagonalForce(particle[i], particle[i + width - 1]);

    // Right lower diagonal neighbour
    if ((i + 1) % width != 0 && i + width + 1 < particleCount) applyDiagonalForce(particle[i], particle[i + width + 1]);
  }

  if (!artificialForces.empty()) applyArtificialForces();
}

void MembraneMolecule::applyArtificialForces() {
  // Apply forces defined for the individual particles within the membrane
  auto& particle = *particles;
  bool allForcesUsedUp{true};
  for (auto& mf : artificialForces) {
    if (mf.timeSpan <= 0) continue;

    particle[mf.particleId].applyForce(mf.force);
    mf.timeSpan--;
    allForcesUsedUp = false;
  }
  if (allForcesUsedUp) artificialForces.clear();
}

[[maybe_unused]] bool MembraneMolecule::indicesAreNeighbours(size_t indexA, size_t indexB) {
  // Assume indexA is the smaller index. Then there are 4 neighbour indices:
  // o x o  indexB is right neighbour (index + 1), lower left neighbour (index + 1 - width)
  // o o o  lower neighbour (index + width) or lower right neighbour (index + 1 + width)
  size_t difference = indexA < indexB ? indexB - indexA : indexA - indexB;

  // We check all 4 cases but simplify the formula to the below one:
  //     difference == width - 1    || difference == width         || difference == width + 1        | - width
  // => difference - width == -1    || difference - width == 0     || difference - width == 1        | + 1
  // => difference + 1 - width == 0 || difference + 1 - width == 1 || difference + 1 - width == 2    | (combine cases)
  // => difference + 1 - width <= 2
  return difference == 1 || difference + 1 - dimensions[dimWidth] <= 2;
}

void MembraneMolecule::applyForce(Particle& p1, Particle& p2, size_t indexP1, size_t indexP2) {
  size_t diff = indexP1 < indexP2 ? indexP2 - indexP1 : indexP1 - indexP2;

  if (diff == 1 || diff == dimensions[dimWidth]) return applyDirectForce(p1, p2);

  if (diff == dimensions[dimWidth] - 1 || diff == dimensions[dimWidth] + 1) return applyDiagonalForce(p1, p2);
  return applyRepulsiveForce(p1, p2);
}

void MembraneMolecule::setMoleculeVectorIndex(size_t index) {
  for (auto& particle : *this) particle.setMolecule(index);
}
