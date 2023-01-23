#include "HarmonicPotentialModel.h"
using ArrayUtils::dotProduct;

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

  x.applySymmetricForce(y, (stiffness / dist) * (dist - 1.41421356237309504880168872 * bondLength) * diff);
}
