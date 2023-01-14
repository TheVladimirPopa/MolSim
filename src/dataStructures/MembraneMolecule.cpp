#include "MembraneMolecule.h"

void MembraneMolecule::linkMembraneParticles() {
  for (auto& particle : particles) {
    // Link direct neighbours
    for (int i = 0; i < 3; i++) {
      for (int offset : {-1, +1}) {
        auto pos = particle.getMembranePosition();
        pos[i] + offset;

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
}

bool MembraneMolecule::positionIsInMembrane(std::array<unsigned long, 3> pos) {
  // unsigned long will underflow for negative values
  if (pos[0] >= dimensions[0] || pos[1] >= dimensions[1] || pos[2] >= dimensions[2]) return false;
  return true;
}

size_t MembraneMolecule::positionToIndex(std::array<unsigned long, 3> pos) {
  return pos[0] + (pos[1] * dimensions[0]) + (pos[2] * dimensions[0] * dimensions[1]);
}

MembraneParticle& MembraneMolecule::getParticleByPosition(std::array<unsigned long, 3> pos) {
  return particles.at(positionToIndex(pos));
}
