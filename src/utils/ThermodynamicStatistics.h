#pragma once
#include "ArrayUtils.h"
#include "dataStructures/IContainer.h"
using ArrayUtils::dotProduct;

/**
 *
 * @param particleContainer
 * @return
 */

double variance(IContainer &particleContainer) {
  double var = 0;

  for (const Particle p : particleContainer) {
    var += dotProduct(p.getX() - p.getInitialX());
  }

  var /= particleContainer.size();

  // particleContainer.forEach(addDotProduct(var)); // templates

  return var;
}

/**
 *
 * @return
 */

double radialDistributionFunction(double delta_r, IContainer &particleContainer, int intervalStart, int intervalEnd) {
  double pi = 3.14;
  int numPairs = 0;
  for (Particle p : particleContainer) {
    for (int i = intervalStart; i <= intervalEnd; i++) {
      double r= dotProduct();
      if () {
        numPairs++;
      }
    }
  }
}
