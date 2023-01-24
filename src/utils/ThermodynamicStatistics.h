#pragma once
#include <map>

#include "ArrayUtils.h"
#include "dataStructures/IContainer.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"
#define PI 3.141592653589793238462643383279502884L
using ArrayUtils::dotProduct;

class ThermodynamicStatistics {
  /**
   * Calculates the diffusion for a LinkedCellsContainer
   * @param particleContainer
   * @return
   */
  static double var(LinkedCellsContainer &particleContainer, int currentTime, int referenceTime) {
    double var = 0;

    for (const Particle &p : particleContainer.particlesVector) {
      var += dotProduct(p.getTimePosition().at(currentTime) - p.getTimePosition().at(referenceTime));
    }

    var /= (double) particleContainer.size();

    return var;
  }
  /**
   * Calculates the diffusion for a VectorContainer
   * @param particleContainer
   * @param currentTime
   * @param referenceTime
   * @return
   */
  static double var(VectorContainer &particleContainer, int currentTime, int referenceTime) {
    double var = 0;

    for (const Particle &p : particleContainer.vector) {
      var += dotProduct(p.getTimePosition().at(currentTime) - p.getTimePosition().at(referenceTime));
    }

    var /= (double) particleContainer.size();

    return var;
  }
  /**
   * Computes the radialDistributionFunction for particles in a LinkedCellsContainer
   * @param delta_r
   * @param particleContainer
   * @param intervalStart
   * @param intervalEnd
   * @return a vector of the local densities
   */
  static std::vector<double> radialDistributionFunction(double delta_r, LinkedCellsContainer &particleContainer,
                                                        int intervalStart, int intervalEnd) {
    std::vector<double> densities;
    for (Particle p1 : particleContainer.particlesVector) {
      for (Particle p2 : particleContainer.particlesVector) {
        if (p1 == p2) {
          continue;
        }
        double r = dotProduct(p1.getX() - p2.getX());
        for (int i = intervalStart; i <= intervalEnd; i++) {
          int numPairs = 0;
          if (r > i && r < i + delta_r) {
            numPairs++;
          }
          double density = numPairs;
          density /= 4 * PI / 3;
          double aux = (i + delta_r);
          aux *= aux * aux;
          aux -= i * i * i;
          density /= aux;
          densities.emplace_back(density);
        }
      }
    }
    return densities;
  }
  /**
   * Computes the radialDistributionFunction of particles in VectorContainer
   * @param delta_r
   * @param particleContainer
   * @param intervalStart
   * @param intervalEnd
   * @return a vector of the local densities
   */
  static std::vector<double> radialDistributionFunction(double delta_r, VectorContainer &particleContainer,
                                                        int intervalStart, int intervalEnd) {
    std::vector<double> densities;
    for (Particle p1 : particleContainer.vector) {
      for (Particle p2 : particleContainer.vector) {
        if (p1 == p2) {
          continue;
        }

        double r = dotProduct(p1.getX() - p2.getX());

        for (int i = intervalStart; i <= intervalEnd; i++) {
          int numPairs = 0;
          if (r > i && r < i + delta_r) {
            numPairs++;
          }
          double density = numPairs;
          density /= 4 * PI / 3;
          double aux = (i + delta_r);
          aux *= aux * aux;
          aux -= i * i * i;
          density /= aux;
          densities.emplace_back(density);
        }
      }
    }
    return densities;
  }
};