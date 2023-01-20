#pragma once
#include "LennardJonesModel.h"

class HarmonicPotentialModel : public LennardJonesModel {
 private:
  const double stiffness;
  const double bondLength;

 public:
  explicit HarmonicPotentialModel(double stiffness_, double bondLength_)
      : LennardJonesModel{1.12246204830937298143353304967917}, stiffness{stiffness_}, bondLength{bondLength_} {};

  /**
   * Applies the default force that is meant to be applied to direct neighbours
   * (= particles that are either horizontal or vertical neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void addDirectForce(Particle& x, Particle& y) const;

  /**
   * Applies the diagonal force that is meant to be applied to diagonal neighbours
   * (= particles that are diagonal neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void addDiagonalForce(Particle& x, Particle& y) const;
};
