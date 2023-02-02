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

std::vector<double> ThermodynamicStatistics::calculateRadialDistributionFunction(double deltaR,
                                                                                 IContainer *particleContainer,
                                                                                 double intervalStart,
                                                                                 double intervalEnd) {
  std::vector<double> densities;
  int size = static_cast<int>(particleContainer->size());
  double i = intervalStart;
  while (i <= intervalEnd) {
    double numPairs = 0;

    for (int j = 0; j < size - 1; j++) {
      Particle p1 = particleContainer->getParticlesRef().at(j);

      for (int k = j + 1; k < size; k++) {
        Particle p2 = particleContainer->getParticlesRef().at(k);

        double r = L2Norm(p1.getX() - p2.getX());
        if (r > i && r <= i + deltaR) {
          numPairs++;
        }
      }
    }
//    if (numPairs == 0) {
//      i += deltaR;
//      continue;
//    }

    double cubeDiff = (i + deltaR);
    cubeDiff *= cubeDiff * cubeDiff;
    cubeDiff -= (i * i * i);
    double density = (3.0 * numPairs) / ((4 * PI * cubeDiff));
    densities.emplace_back(density);

    i += deltaR;
  }

  return densities;
}