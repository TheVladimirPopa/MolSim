//
// Created by leo on 09.11.22.
//

#include "Simulation.h"

#include "spdlog/spdlog.h"

void Simulation::simulate(IModel &model, IContainer &particles, IWriter &fileWriter, Thermostat &thermostat,
                          double gravitationalConstant) {
  spdlog::info("Simulation is starting...");
  double current_time = startTime;
  int iteration = 0;

  // Pass methods of model as lambdas. More lightweight than std::function.
  using P = Particle &;
  std::function<void(P)> updateX{[&model](P p) { model.updateX(std::forward<P>(p)); }};
  std::function<void(P)> updateV{[&model](P p) { model.updateV(std::forward<P>(p)); }};
  std::function<void(P)> updateF{[](P p) { p.updateForces(); }};
  std::function<void(P, P)> addForces{
      [&model](P p1, P p2) { model.addForces(std::forward<P>(p1), std::forward<P>(p2)); }};
  std::function<void(P)> applyGravity{[&gravitationalConstant](P p) { p.f[1] += p.m * gravitationalConstant; }};
  // Initialize the container to the temperature
  thermostat.initializeTemperature();

  // Initialize the force so that we know the current force for the first loop
  particles.forEach(updateF);
  particles.forEachPair(addForces);
  if (gravitationalConstant != 0.0) particles.forEach(applyGravity);

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < endTime) {
    particles.forEach(updateX);
    particles.forEach(updateF);
    particles.forEachPair(addForces);
    if (gravitationalConstant != 0.0) particles.forEach(applyGravity);
    particles.forEach(updateV);

    if (iteration % thermostat.getPeriodLength() == 0 && iteration != 0) {
      thermostat.applyThermostat();
    }

    if (iteration % writeOutFrequency == 0) {
      fileWriter.writeFile(filename, iteration, particles);
    }

    current_time += deltaT;
    iteration++;
  }
}
