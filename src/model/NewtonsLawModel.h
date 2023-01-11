#pragma once
#include "IModel.h"
class NewtonsLawModel : public IModel {
 public:
  virtual ~NewtonsLawModel() = default;

  /**
   * Add the forces both particles apply upon each other according to
   * Newton's law of gravity.
   * @param p1 The particle which influences particle p2
   * @param p2 The particle which influences particle p1
   */
  void addForces(Particle &p1, Particle &p2) const override;
};