#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::dotProduct;

void LennardJonesModel::addForces(Particle &p1, Particle &p2) {
  double length = dotProduct(p1.x - p2.x);
  ++comparisons;
  if (length >= cutOffRadiusSquared) return;
  ++hits;

  // Calculate (sigma⁶/length³)
  double sigma = (p1.sigma + p2.sigma) / 2;
  double sig_len = sigma * sigma / length;  // (sigma²/length)
  sig_len *= sig_len * sig_len;             // (sigma²/length)³

  // This is the force formula from worksheet 2 slightly reformulated for better numerical accuracy.
  double epsilon = p1.epsilon == p2.epsilon ? p1.epsilon : std::sqrt(p1.epsilon * p2.epsilon);
  auto force = ((-24 * epsilon) / length) * (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}
void LennardJonesModel::setCutOffRadius(double cutOffRadius) {
  LennardJonesModel::cutOffRadiusSquared = cutOffRadius * cutOffRadius;
}
