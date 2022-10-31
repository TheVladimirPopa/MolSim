#pragma once
#include "utils/Interfaces.h"
class NewtonsLawModel : public IModel {
 public:
  virtual ~NewtonsLawModel() {};
  virtual void iterate(ParticleContainer &particleContainer, double const& delta_t) override;

  void calculateF(ParticleContainer &particles);

  void calculateX(ParticleContainer &particles, double const& delta_t);

  void calculateV(ParticleContainer &particles, double const& delta_t);
};