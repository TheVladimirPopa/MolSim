#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::dotProduct;

void LennardJonesModel::addForces(Particle &p1, Particle &p2) {
  double length_sqr = dotProduct(p1.x - p2.x);
  ++comparisons;
  if (length_sqr >= cutOffRadiusSquared) return;
  ++hits;

  // Calculate (sigma⁶/length⁶)
  double sigma = p1.sigma == p2.sigma ? p1.sigma : (p1.sigma + p2.sigma) / 2;
  double sig_len = sigma * sigma / length_sqr;  // (sigma²/length²)
  sig_len *= sig_len * sig_len;                 // (sigma²/length²)³

  // This is the force formula from worksheet 2 slightly reformulated for better numerical accuracy.
  double epsilon = p1.epsilon == p2.epsilon ? p1.epsilon : std::sqrt(p1.epsilon * p2.epsilon);
  auto force = ((-24 * epsilon) / length_sqr) * (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);

  p1.applySymmetricForce(p2, force);
}
void LennardJonesModel::setCutOffRadius(double cutOffRadius) {
  LennardJonesModel::cutOffRadiusSquared = cutOffRadius * cutOffRadius;
}
