#pragma once

#include "dataStructures/IContainer.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"

class ThermodynamicStatistics {
 private:
  /**
   * Calculates the position of the particles, also taking periodic boundaries into account
   * @param particleContainer
   * @param oldPosition
   * @param boundariesCrossed
   * @return
   */
  static std::array<double, 3> getTruePosition(LinkedCellsContainer &particleContainer,
                                               std::array<double, 3> oldPosition, std::array<int, 3> boundariesCrossed);

  /**
   * Used to determine the class of an object (similar to instanceof in Java)
   * @tparam Base
   * @tparam T
   * @param ptr
   * @return
   */
  template <typename Base, typename T>
  inline bool instanceof (const T *ptr) {
    return dynamic_cast<const Base *>(ptr) != nullptr;
  }

 public:
  /**
   * Calculates the diffusion for the particles located in a LinkedCellsContainer
   * @param particleContainer contains the particles
   * @return
   */
  double calculateVariance(IContainer *particleContainer);
  /**
   * Computes the calculateRadialDistributionFunction for particles in a LinkedCellsContainer
   * @param deltaR
   * @param particleContainer
   * @param intervalStart
   * @param intervalEnd
   * @return a vector of the local densities
   */
  static std::vector<double> calculateRadialDistributionFunction(double deltaR, IContainer *particleContainer,
                                                        double intervalStart, double intervalEnd);
};