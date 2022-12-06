#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;

void LennardJonesModel::addForces(Particle &p1, Particle &p2) const {
  double length = L2Norm(p1.x - p2.x);
  if (length >= cutOffRadius) return;

  // Calculate (sigma/length)^6
  double sig_len = sigma / length;        // (sigma/length)^1
  sig_len = sig_len * sig_len * sig_len;  // (sigma/length)^3
  sig_len = sig_len * sig_len;            // (sigma/length)^6

  // This is the force formula from worksheet 2 slightly reformulated for better
  // numerical accuracy.
  auto force = ((-24 * epsilon) / (length * length)) *
               (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}