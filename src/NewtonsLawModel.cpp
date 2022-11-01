#include "NewtonsLawModel.h"
#include <iostream>
#include <cmath>

/**
 * todo: document this
 */
void NewtonsLawModel::addForces(Particle &p1, Particle &p2) {
  // Todo: this is Leos version. Maybe wait for Vladimirs version.
  std::array<double, 3> p1Force{0.0, 0.0, 0.0};

  double quotient =
      p1.getM() * p2.getM() * pow(computeSquareDistance(p1, p2), -1.5);
  for (size_t dimension = 0; dimension < p1.getX().size(); ++dimension) {
    double force = quotient * (p2.getX()[dimension] - p1.getX()[dimension]);
    p1.f[dimension] += force;
    p2.f[dimension] -= force; // Newtons 3rd law
  }
}

double NewtonsLawModel::computeSquareDistance(const Particle &p1, const Particle &p2) {
  double sum{0.0};
  for (size_t dimension = 0; dimension < p1.getX().size(); ++dimension) {
    sum += pow(p1.getX()[dimension] - p2.getX()[dimension], 2);
  }
  return sum;
}