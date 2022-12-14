#pragma once
#include "IModel.h"

class LennardJonesModel : public IModel {
 private:
  constexpr static double epsilon = 5.0;
  constexpr static double sigma = 1.0;

  /// The max distance between two particles where are force is applied
  double cutOffRadius;

 public:
  void setCutOffRadius(double cutOffRadius);

 public:
  /**
   * Instantiates a force calculation model based on LennardJonesModel
   * @param cutOffRadius_ The max distance between two particles where are force
   * is applied, greater distances between particles the force is considered 0
   */
  explicit LennardJonesModel(double cutOffRadius_)
      : cutOffRadius{cutOffRadius_} {}

  virtual ~LennardJonesModel() = default;

  /**
   * Add the forces both particles apply upon each based on the
   * Lennard-Jones potential.
   * @param p1 The particle which influences particle p2
   * @param p2 The particle which influences particle p1
   */
  void addForces(Particle &p1, Particle &p2) const override;

  double getEpsilon() const { return epsilon; }

  double getSigma() const { return sigma; }
};