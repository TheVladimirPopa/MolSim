#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

void LennardJonesModel::addForces(Particle &p1, Particle &p2) {
  double length = L2Norm(p1.x - p2.x);
  ++comparisons;
  if (length >= cutOffRadius) return;
  ++hits;

  // Calculate (sigma/length)^6
  double sig_len = (p1.sigma + p2.sigma) / (2 * length);  // (sigma/length)^1
  sig_len = sig_len * sig_len * sig_len;                  // (sigma/length)^3
  sig_len = sig_len * sig_len;                            // (sigma/length)^6

  // This is the force formula from worksheet 2 slightly reformulated for better numerical accuracy.
  double epsilon = p1.epsilon == p2.epsilon ? p1.epsilon : std::sqrt(p1.epsilon * p2.epsilon);
  auto force = ((-24 * epsilon) / (length * length)) * (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}
void LennardJonesModel::setCutOffRadius(double cutOffRadius) { LennardJonesModel::cutOffRadius = cutOffRadius; }
