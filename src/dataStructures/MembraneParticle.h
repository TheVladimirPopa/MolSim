#pragma once
#include <vector>

#include "dataStructures/Particle.h"

class MembraneParticle : public Particle {
 private:
  std::vector<MembraneParticle*> directNeighbours;
  std::vector<MembraneParticle*> diagonalNeighbours;
  std::array<size_t, 3> membranePosition{};

 public:
  MembraneParticle(std::array<size_t, 3> membranePosition, std::array<double, 3> x_arg, std::array<double, 3> v_arg = {0.0, 0.0, 0.0}, double m_arg = 1.0, int type = 0);

  void addDirectNeighbourUnique(MembraneParticle* particle);

  void addDiagonalNeighbourUnique(MembraneParticle* particle);

  [[nodiscard]] std::vector<MembraneParticle*> getDirectNeighbours() const { return directNeighbours; }

  [[nodiscard]] std::vector<MembraneParticle*> getDiagonalNeighbours() const { return diagonalNeighbours; }

  std::array<size_t, 3> getMembranePosition() const { return membranePosition; };
};
