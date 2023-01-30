#include "SmoothedLennardJonesModel.h"

#include "LennardJonesModel.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::dotProduct;
using ArrayUtils::L2Norm;

void SmoothedLennardJonesModel::addForces(Particle &p1, Particle &p2) const {
  double length_sqr = dotProduct(p1.x - p2.x);
  //++comparisons;
  if (length_sqr >= radius_c * radius_c) return;
  //++hits;

  // Calculate (sigma⁶/length⁶)
  double sigma = p1.sigma == p2.sigma ? p1.sigma : (p1.sigma + p2.sigma) / 2;
  // This is the force formula from worksheet 2 slightly reformulated for better numerical accuracy.
  double epsilon = p1.epsilon == p2.epsilon ? p1.epsilon : std::sqrt(p1.epsilon * p2.epsilon);

  std::array<double, 3> force{};

  double distance = L2Norm(p1.x - p2.x);

  if (distance <= radius_l) {
    double sig_len = sigma * sigma / length_sqr;  // (sigma²/length²)
    sig_len *= sig_len * sig_len;                 // (sigma²/length²)³
    force = ((-24 * epsilon) / length_sqr) * (sig_len * (1 - (2 * sig_len))) * (p1.x - p2.x);
  } else if (radius_l <= distance && distance <= radius_c) {
    // x_j - x_i
    auto diff = p2.x - p1.x;

    // -24 * sigma^(6) * epsilon
    auto potentialConst = -24 * sigma * sigma * sigma;
    potentialConst *= sigma * sigma * sigma;
    potentialConst *= epsilon;
    // d_ij^(14)
    auto distTo14 = distance;
    for (int i = 2; i <= 14; i++) {
      distTo14 *= distance;
    }
    // (r_c * r_l)^3
    auto radialDiffCubed = radius_c - radius_l;
    radialDiffCubed *= radialDiffCubed * radialDiffCubed;
    distTo14 *= radialDiffCubed;

    // (-24 * sigma^(6) * epsilon) / (d_ij^(14) * (r_c * r_l)^3)
    potentialConst /= distTo14;

    potentialConst *= (radius_c - distance);

    // r_c^2 * (2 * sigma^6 - d_ij^6)
    auto rc_squared = radius_c * radius_c;
    auto doubleSigmaTo6 = 2 * sigma * sigma * sigma;
    doubleSigmaTo6 *= sigma * sigma * sigma;
    auto distanceTo6 = distance * distance * distance;
    distanceTo6 *= distanceTo6;
    rc_squared *= (doubleSigmaTo6 - distanceTo6);

    // r_c * (3 * r_l - d_ij) * (d_ij^6 - 2 * sigma^6)
    auto rc_TimesRadDiff_TimesSigmaDiff = radius_c * (3 * radius_l - distance);
    rc_TimesRadDiff_TimesSigmaDiff *= (distanceTo6 - doubleSigmaTo6);

    // d_ij * (5 * r_l * sigma^6 - 2 * r_l * d_ij^6 - 3 * sigma^6 * d_ij + d_ij^7)
    auto distTimesDiff = 5 * radius_l * doubleSigmaTo6 / 2 - 2 * radius_l * distanceTo6 -
                         3 * doubleSigmaTo6 / 2 * distance + distanceTo6 * distance;
    distTimesDiff *= distance;

    potentialConst *= (rc_squared + rc_TimesRadDiff_TimesSigmaDiff + distTimesDiff);
    diff = potentialConst * diff;

    force = diff;
  } else {
    force = {0., 0., 0.};
  }

  p1.f = p1.f + force;
  p2.f = p2.f - force;
}
