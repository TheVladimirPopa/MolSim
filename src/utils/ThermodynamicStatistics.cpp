#include "ThermodynamicStatistics.h"

#include "ArrayUtils.h"
#include "dataStructures/IContainer.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"
#define PI 3.14159265358979323846
using ArrayUtils::L2Norm;

std::array<double, 3> ThermodynamicStatistics::getTruePosition(LinkedCellsContainer &particleContainer,
                                                               std::array<double, 3> oldPosition,
                                                               std::array<int, 3> boundariesCrossed) {
  for (int i = 0; i < 3; i++) {
    oldPosition[i] += boundariesCrossed[i] * particleContainer.cubeSize[i];
  }
  return oldPosition;
}

double ThermodynamicStatistics::calculateVariance(IContainer *particleContainer) {
  double var = 0;

  if (instanceof <VectorContainer>(particleContainer))
    for (Particle &p : particleContainer->getParticlesRef()) {
      auto dist = L2Norm(p.getX() - p.getLastPosition());
      dist *= dist;
      var += dist;
    }
  else {
    auto &lcc = dynamic_cast<LinkedCellsContainer &>(*particleContainer);
    for (Particle &p : particleContainer->getParticlesRef()) {
      auto dist = L2Norm(ThermodynamicStatistics::getTruePosition(lcc, p.getX(), p.getPeriodicBoundariesCrossed()) -
                         p.getLastPosition());
      dist *= dist;
      var += dist;
      p.getPeriodicBoundariesCrossed() = {0, 0, 0};
    }
  }

  var /= static_cast<double>(particleContainer->size());

  return var;
}

std::vector<double> ThermodynamicStatistics::calculateRadialDistributionFunction(double delta_r,
                                                                                 IContainer *particleContainer,
                                                                                 double intervalStart,
                                                                                 double intervalEnd) {
  std::vector<double> densities;
  for (Particle p1 : particleContainer->getParticlesRef()) {
    for (Particle p2 : particleContainer->getParticlesRef()) {
      if (p1 == p2) {
        continue;
      }
      double r = L2Norm(p1.getX() - p2.getX());
      double i = intervalStart;
      while (i <= intervalEnd) {
        double numPairs = 0;
        if (r > i && r <= i + delta_r) {
          numPairs++;
        }
        if (numPairs == 0) {
          continue;
        }
        double cubeDiff = (i + delta_r);
        cubeDiff *= cubeDiff * cubeDiff;
        cubeDiff -= (i * i * i);
        double density = (3.0 * numPairs) / ((4 * PI * cubeDiff));
        densities.emplace_back(density);
        i += delta_r;
      }
    }
  }
  return densities;
}