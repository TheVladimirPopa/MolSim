//
// Created by leo on 09.11.22.
//
#include "Simulation.h"

#include "Configuration.h"
#include "outputWriter/CheckpointFileWriter.h"
#include "outputWriter/StatisticsWriter.h"
#include "spdlog/spdlog.h"
#define PROGRESS_MSG_COUNT 15

void Simulation::simulate(IModel& model, IContainer& particles, IWriter& fileWriter, Thermostat& thermostat,
                          double gravitationalConstant, bool checkpointing, bool statistics,
                          CheckpointFileWriter& cpfWriter, StatisticsWriter& statWriter) {
  spdlog::info("Simulation is starting...");
  size_t expectedIterationCount = std::ceil((endTime - startTime) / deltaT);
  size_t printProgressInterval =
      ((expectedIterationCount / PROGRESS_MSG_COUNT) / writeOutFrequency) * writeOutFrequency;

  double current_time = startTime;
  int iteration = 0;
  size_t updateCount = 0;

  // Pass methods of model as lambdas. More lightweight than std::function.
  using P = Particle&;
  std::function<void(P)> updateX{[&model, &updateCount](P p) {
    model.updateX(std::forward<P>(p));
    ++updateCount;
  }};
  std::function<void(P)> updateV{[&model](P p) { model.updateV(std::forward<P>(p)); }};
  std::function<void(P)> updateF;
  if (gravitationalConstant == 0) {
    updateF = [](P p) { p.updateForces(); };
  } else {
    updateF = [gravitationalConstant](P p) {
      p.updateForces();
      p.f[2] += p.m * gravitationalConstant;
    };
  }

  std::function<void(P, P)> addForces{
      [&model](P p1, P p2) { model.addForces(std::forward<P>(p1), std::forward<P>(p2)); }};
  std::function<void(P p)> registerLastPosition{[](Particle& p) { p.lastPosition = p.getX(); }};

  // Enable structure physics handling on demand
  if (particles.containsStructures()) {
    addForces = [&model, &particles](P p1, P p2) {
      if (p1.isInStructure() && p1.getStructure() == p2.getStructure())
        particles.getStructureVectorRef()[p1.getStructure()].applyForce(p1, p2, p1.getId(), p2.getId());
      else
        model.addForces(std::forward<P>(p1), std::forward<P>(p2));
    };
  }

  // Initialize the container to the temperature
  if (thermostat.getPeriodLength() != 0) thermostat.initializeTemperature();

  // Initialize the force so that we know the current force for the first loop
  if (particles.containsStructures())
    for (auto& mol : particles.getStructureVectorRef()) mol.applyArtificialForces();

  particles.forEach(updateF);
  particles.forEachPair(addForces);

  if (statistics) {
    particles.forEach(registerLastPosition);
  }

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < endTime) {
    particles.forEach(updateX);
    particles.forEach(updateF);
    particles.forEachPair(addForces);
    particles.forEach(updateV);

    if (thermostat.getPeriodLength() != 0 && iteration % thermostat.getPeriodLength() == 0 && iteration != 0) {
      thermostat.applyThermostat();
    }

    current_time += deltaT;

    if (iteration % writeOutFrequency == 0) {
      fileWriter.writeFile(filename, iteration, particles);

      if ((printProgressInterval == 0 || iteration % printProgressInterval == 0) && startTime < endTime)
        spdlog::info("Completed {} iterations. ({:0.1f}%)", iteration, (100.0 * iteration) / expectedIterationCount);
    }

    // Every 1000th iteration the statistics are being written
    if (statistics && iteration % statWriter.getFrequency() == 0 && iteration != 0) {
      statWriter.writeFile(statWriter.getFilename(), iteration, particles);
      particles.forEach(registerLastPosition);
    }

    current_time += deltaT;
    iteration++;
  }
  moleculeUpdateCount = updateCount;

  spdlog::info("Completed {} iterations. (100.0%)", iteration);
  if (checkpointing) {
    spdlog::info("Writing checkpoint.");
    cpfWriter.writeFile(cpfWriter.getOutputFile(), iteration, particles);
  }
}
