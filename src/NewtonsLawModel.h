#pragma once
#include "IModel.h"
class NewtonsLawModel : public IModel {
 private:
  double computeSquareDistance(const Particle &p1, const Particle &p2) const;

 public:
  virtual ~NewtonsLawModel() {};

  /**
   * Add forces to both particles based on Newtons law of gravitation.
   * @param p1 Particle 1
   * @param p2 Particle 2
   */
  void addForces(Particle &p1, Particle &p2) const override;
};