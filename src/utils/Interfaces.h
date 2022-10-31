#pragma once

#include "ParticleContainer.h"

enum class Model {
  NewtonsLaw
};

class IModel
{
 public:
  virtual ~IModel() {}
  virtual void iterate(ParticleContainer &particleContainer, double const& delta_t) = 0;
};





