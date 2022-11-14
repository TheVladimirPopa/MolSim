#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;


void LennardJonesModel::addForces(Particle &p1, Particle &p2) const {
  // Calculate (sigma/length)^6
  double length = L2Norm(p1.x - p2.x);
  double x = sigma / length;   // (sigma/length)^1
  x = x * x * x;               // (sigma/length)^3
  x = x * x;                   // (sigma/length)^6

  // This is the force formula from worksheet 2 slightly reformulated for better
  // numerical accuracy.
  auto force =
      ((24 * epsilon) / (length * length)) * (x * (1 - (2 * x))) * (p1.x - p2.x);

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}