#include "SmoothedLennardJonesModel.h"

#include "utils/ArrayUtils.h"
using ArrayUtils::dotProduct;

void SmoothedLennardJonesModel::addForces(Particle &p1, Particle &p2) const {
  double length = dotProduct(p1.x - p2.x);  // d_ij
  //++comparisons;
  if (length >= radius_c * radius_c) return;
  //++hits;

  // Calculate (sigma⁶/length³)
  double sigma = p1.sigma == p2.sigma ? p1.sigma : (p1.sigma + p2.sigma) / 2;
  double sig_len = sigma * sigma / length;  // (sigma²/length)
  sig_len *= sig_len * sig_len;             // (sigma²/length)³

  // This is the force formula from worksheet 2 slightly reformulated for better numerical accuracy.
  double epsilon = p1.epsilon == p2.epsilon ? p1.epsilon : std::sqrt(p1.epsilon * p2.epsilon);

  std::array<double, 3> force{};

  if (length <= radius_l) {
    force = ((-24 * epsilon) / length) * (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);
  } else if (radius_l <= length && length <= radius_c) {
    // x_j - x_i
    auto d = p2.x - p1.x;

    // -24 * sigma^(6) * epsilon
    auto d1 = -24 * sigma * sigma * sigma;
    d1 *= sigma * sigma * sigma;
    d1 *= epsilon;
    // d_ij^(14)
    auto d2 = length;
    for (int i = 2; i <= 14; i++) {
      d2 *= length;
    }
    // (r_c * r_l)^3
    auto d3 = radius_c - radius_l;
    d3 *= d3 * d3;
    d2 *= d3;

    // (-24 * sigma^(6) * epsilon) / (d_ij^(14) * (r_c * r_l)^3)
    d1 /= d2;

    d1 *= (radius_c - length);

    // r_c^2 * (2 * sigma^6 - d_ij^6)
    auto d4 = radius_c * radius_c;
    auto d5 = 2 * sigma * sigma * sigma;
    d5 *= sigma * sigma * sigma;
    auto d6 = length * length * length;
    d6 *= length * length * length;
    d4 *= (d5 - d6);

    // r_c * (3 * r_l - d_ij) * (d_ij^6 - 2 * sigma^6)
    auto d7 = radius_c * (3 * radius_l - length);
    d7 *= (d6 - d5);

    // d_ij * (5 * r_l * sigma^6 - 2 * r_l * d_ij^6 - 3 * sigma^6 * d_ij + d_ij^7)
    auto d8 = 5 * radius_l * d5 / 2 - 2 * radius_l * d6 - 3 * d5 / 2 * length + d6 * length;
    d8 *= length;

    d1 *= (d4 + d7 + d8);
    d = d1 * d;

    force = d;
  } else {
    force = {0., 0., 0.};
  }

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}
