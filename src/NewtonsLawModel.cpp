#include "NewtonsLawModel.h"
#include <iostream>

void NewtonsLawModel::iterate(ParticleContainer &particleContainer, double const& delta_t)
{
  // calculate new x
  calculateX(particleContainer, delta_t);
  // calculate new f
  calculateF(particleContainer);
  // calculate new v
  calculateV(particleContainer, delta_t);
}

/**
 * calculate the force for all particles
 */
void NewtonsLawModel::calculateF(ParticleContainer &particles) {
  for (auto &p1 : particles) {
    p1.resetForceIteration();
    for (auto &p2 : particles) {
      if (!(p1 == p2)) p1.addForce(p2);
    }
  }
}

/**
 * calculate the position for all particles
 */
void NewtonsLawModel::calculateX(ParticleContainer &particles, double const& delta_t) {
  for (auto &p : particles) {
    p.updateLocation(delta_t);
    std::cout << p.toString() << '\n';
  }
}

/**
 * calculate the position for all particles
 */
void NewtonsLawModel::calculateV(ParticleContainer &particles, double const& delta_t) {
  for (auto &p : particles) {
    p.updateVelocity(delta_t);
  }
}
