#pragma once
#include "IModel.h"

class SmoothedLennardJonesModel : public IModel {
 private:
  double radius_c;
  double radius_l;

  size_t hits{};
  size_t comparisons{};

 public:
  /**
   * Instantiates a force calculation model based on the SmoothedLennardJonesModel
   * @param cutOffRadius The max distance between two particles where forces are applied, greater distances between
   * particles have the force 0
   * @param radius_l
   */
  explicit SmoothedLennardJonesModel(double cutOffRadius, double radius_l)
      : radius_c{cutOffRadius * cutOffRadius}, radius_l{radius_l} {};
  ~SmoothedLennardJonesModel() = default;

  /**
   * Add the forces both particles apply upon each based on the Smoothed Lennard-Jones potential.
   * @param p1 The particle which influences particle p2
   * @param p2 The particle which influences particle p1
   */
  void addForces(Particle &p1, Particle &p2) const override;

  size_t getHits() const { return hits; }
  size_t getComparisons() const { return comparisons; }
};
