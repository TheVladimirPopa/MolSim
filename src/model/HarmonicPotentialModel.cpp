#include "HarmonicPotentialModel.h"
using ArrayUtils::dotProduct;
#define SQRT_2 1.41421356237309504880168872

void HarmonicPotentialModel::addDirectForce(Particle& x, Particle& y) const {
  auto diff = y.getX() - x.getX();
  auto dist = dotProduct(diff);
  if (dist >= harmonicCutOffSquared) return;
  dist = std::sqrt(dist);

  x.applySymmetricForce(y, (stiffness / dist) * (dist - bondLength) * diff);
}

void HarmonicPotentialModel::addDiagonalForce(Particle& x, Particle& y) const {
  auto diff = y.getX() - x.getX();
  auto dist = dotProduct(diff);
  if (dist >= harmonicCutOffSquared) return;
  dist = std::sqrt(dist);

  x.applySymmetricForce(y, (stiffness / dist) * (dist - SQRT_2 * bondLength) * diff);
}
