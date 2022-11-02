#include "NewtonsLawModel.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include <cmath>


void NewtonsLawModel::addForces(Particle &p1, Particle &p2) const {
  auto force =
      ((p1.m * p2.m) / pow(L2Norm(p1.x - p2.x), 3)) * (p2.x - p1.x);
  p1.f = p1.f + force;
  p2.f = p2.f - force; // Netwon's 3rd law
}