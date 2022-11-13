#include "LennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;


void LennardJonesModel::addForces(Particle &p1, Particle &p2) const {
  // Calculate (sigma/length)^6
  double l = L2Norm(p1.x - p2.x);
  double x = sigma / l;   // x^1 - we need x^6
  x = x * x * x;          // x^3
  x = x * x;              // x^6

  auto force = ((24 * epsilon) / (l * l)) * (x * (1 - (2 * x))) * (p1.x - p2.x);

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}