//
// Created by leo on 09.11.22.
//

#include "Simulation.h"

void Simulation::simulate(IModel const &model, ParticleContainer &particles,
                          IWriter &fileWriter) {
  double current_time = startTime;
  int iteration = 0;

  // This is wrapper code that hopefully we can replace in the future
  std::function<void(Particle &)> updateX = [&model](Particle &p) {
    model.updateX(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateV = [&model](Particle &p) {
    model.updateV(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateF = [](Particle &p) {
    p.updateForces();
  };

  std::function<void(Particle &, Particle &)> addForces = [&model](
                                                              Particle &p1,
                                                              Particle &p2) {
    model.addForces(std::forward<Particle &>(p1), std::forward<Particle &>(p2));
  };

  // Initialize the force so that we know the current force for the first loop
  particles.forEach(updateF);
  particles.forEachPair(addForces);

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < endTime) {
    particles.forEach(updateX);

    particles.forEach(updateF);
    particles.forEachPair(addForces);

    particles.forEach(updateV);

    if (iteration % writeOutFrequency == 0) {
      fileWriter.writeFile(filename, iteration, particles);
    }

    current_time += deltaT;
    iteration++;
  }
}

void Simulation::setEndTime(double endTime) { Simulation::endTime = endTime; }
void Simulation::setDeltaT(double deltaT) { Simulation::deltaT = deltaT; }
void Simulation::setIterationsPerWrite(unsigned int writeOutFrequency) {
  Simulation::writeOutFrequency = writeOutFrequency;
}
void Simulation::setFilename(const std::string &filename) {
  Simulation::filename = filename;
}
