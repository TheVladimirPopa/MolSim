#pragma once
#include "dataStructures/MembraneStructure.h"
#include <array>

class NanoTubeStructure : public MembraneStructure {
 public:
  NanoTubeStructure(std::array<size_t, 3> dimensions, double stiffness, double bondLength, double cutOff,
                    std::vector<Particle>& particles);

  /**
   * Applies the correct inner structure force selected based on indices.
   * @param p1 Particle 1
   * @param p2 Particle 2
   * @param indexP1 Index of particle 1
   * @param indexP2 Index of particle 2
   */
  void applyForce(Particle& p1, Particle& p2) override;
};
