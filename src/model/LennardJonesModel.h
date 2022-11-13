#pragma once
#include "IModel.h"

class LennardJonesModel : public IModel {
 private:
  constexpr static double epsilon = 5.0;
  constexpr static double sigma = 1.0;

  double LennardJonesPotential(Particle const &p1, Particle const &p2) const;

 public:
  virtual ~LennardJonesModel() = default;

  /**
   * Add the forces both particles apply upon each other according to
   * Newton's law of gravity.
   * @param p1 The particle which influences particle p2
   * @param p2 The particle which influences particle p1
   */
  void addForces(Particle &p1, Particle &p2) const override;

  double getEpsilon() const { return epsilon; }

  double getSigma() const { return sigma; }
};