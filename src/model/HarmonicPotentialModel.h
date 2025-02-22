#pragma once
#include "LennardJonesModel.h"

class HarmonicPotentialModel : public LennardJonesModel {
 private:
  /// The stiffness constant of the membrane we model
  const double stiffness;

  /// The average bond length of a molecule pair of the membrane we model
  const double bondLength;

  /// The radius at which direct and diagonal forces are applied at. Cutting this of will cause membranes to tear.
  const double harmonicCutOffSquared;

 public:
  explicit HarmonicPotentialModel(double stiffness_, double bondLength_, double cutOff = 1e9)
      : LennardJonesModel{1.12246204830937298143353304967917},
        stiffness{stiffness_},
        bondLength{bondLength_},
        harmonicCutOffSquared{cutOff * cutOff} {};

  /**
   * Applies the membrane default force that is meant to be applied to direct neighbours
   * (= particles that are either horizontal or vertical neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void addDirectForce(Particle& x, Particle& y) const;

  /**
   * Applies the membrane diagonal force that is meant to be applied to diagonal neighbours
   * (= particles that are diagonal neighbours).
   * @param x The first particle that influences the second particle
   * @param y The second particle that influences the first particle
   */
  void addDiagonalForce(Particle& x, Particle& y) const;
};
