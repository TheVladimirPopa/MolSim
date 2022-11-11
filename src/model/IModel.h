#pragma once

#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "utils/ArrayUtils.h"

class IModel {
 private:
  double delta_t;

 public:
  /**
   * Calculate position of particle
   * @param p The particle that gets its location changed
   * @param delta_t The time period in which the velocity affects the location
   */
  virtual void updateX(Particle &p) const {
    p.x = p.x + ((delta_t * p.v) + delta_t * delta_t * ((1 / (2 * p.m)) * p.f));
  }

  /**
   * Calculate velocity of particle
   * @param p The particle that gets its velocity changed
   * @param delta_t The time period in which the forces accelerate the particle
   */
  virtual void updateV(Particle &p) const {
    p.v = p.v + (delta_t * ((1 / (2 * p.m)) * (p.f + p.old_f)));
  }

  /**
   * Let the forces of two particles interact. Forces simply add up on both
   * particles.
   * @param p1 The particle that gets influenced by particle p2
   * @param p2 The particle that gets influenced by particle p1
   * DO NOT FORGET TO RESET THE CURRENT FORCES ON THE PARTICLE AT THE START OF
   * EACH ITERATION.
   */
  virtual void addForces(Particle &p1, Particle &p2) const = 0;

  /**
   * Sets the time period for the individual simulation iterations.
   * @param d_t Simulated length of a single iteration
   */
  void setDeltaT(double const &d_t) { delta_t = d_t; }
};