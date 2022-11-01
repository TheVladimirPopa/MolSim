#pragma once

#include "Particle.h"
#include "ParticleContainer.h"

enum class Model {
  NewtonsLaw
};

class IModel
{
 private:
  double delta_t;


 public:
  virtual ~IModel() {}

  virtual void setDeltaT(double const& d_t) {
    delta_t = d_t;
  }

  /**
   * Calculate position of particle
   * @param p The particle that gets its location changed
   * @param delta_t The time period in which the velocity affects the location
   */
  virtual void updateX(Particle &p) {
    // Todo: this is Leos version. Maybe wait for Vladimirs version.
    for (size_t dimension = 0; dimension < p.getX().size(); ++dimension) {
      p.x[dimension] += (delta_t * p.v[dimension]) +
                  ((delta_t * delta_t * p.getF()[dimension]) / (2 * p.getM()));
    }
  }

  /**
   * Calculate velocity of particle
   * @param p The particle that gets its velocity changed
   * @param delta_t The time period in which the forces accelerate the particle
   */
  virtual void updateV(Particle &p) {
    // Todo: this is Leos version. Maybe wait for Vladimirs version.
    for (size_t dimension = 0; dimension < p.getX().size(); ++dimension) {
      p.v[dimension] += (delta_t * (p.old_f[dimension] + p.f[dimension]) / (2 * p.m));
    }
  }

  /**
   * Let the forces of two particles interact. Forces simply add up on both
   * particles.
   * @param p1 The particle that gets influenced by particle p2
   * @param p2 The particle that gets influenced by particle p1
   * DO NOT FORGET TO RESET THE CURRENT FORCES ON THE PARTICLE AT THE START OF
   * EACH ITERATION.
   */
  virtual void addForces(Particle &p1, Particle &p2) = 0;

};





