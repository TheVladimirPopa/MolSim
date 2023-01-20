#include "HarmonicPotentialModel.h"
using ArrayUtils::L2Norm;

void HarmonicPotentialModel::addDirectForce(Particle& x, Particle& y) const {
  auto diff = y.getX() - x.getX();
  auto dist = L2Norm(diff);

  auto force = stiffness * (1 / dist) * (dist - bondLength) * diff;
  x.applySymmetricForce(y, force);
}

void HarmonicPotentialModel::addDiagonalForce(Particle& x, Particle& y) const {
  auto diff = y.getX() - x.getX();
  auto dist = L2Norm(diff);

  auto force = stiffness * (1 / dist) * (dist - 1.41421356237309504880168872 * bondLength) * diff;

  x.applySymmetricForce(y, force);
}
