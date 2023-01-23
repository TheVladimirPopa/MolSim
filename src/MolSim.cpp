
#include <getopt.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Configuration.h"
#include "Simulation.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/Particle.h"
#include "dataStructures/VectorContainer.h"
#include "inputReader/FileReader.h"
#include "inputReader/XMLFileReader/XMLParser.h"
#include "model/LennardJonesModel.h"
#include "model/Thermostat.h"
#include "outputWriter/NoWriter.h"
#include "outputWriter/VTKWriter.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "utils/SimulationUtils.h"

// --------------------------------------
//                 Main
// --------------------------------------
int main(int argc, char* argsv[]) {
  // Print help when no arguments or options are present
  if (argc == 1) {
    Configuration::printUsage();
    return 1;
  }

  // 1. Load config & configure logging (e.g. no logging when performance measurement)
  Configuration config = Configuration::parseOptions(argc, argsv);

  if (config.getInputType() == InputType::XML) {
    bool xmlInputOk = config.tryParseXml();

    if (!xmlInputOk) exit(EXIT_FAILURE);
  }

  ConfigurationUtils::configureLogging(config.getLogLevel(), !config.isLoggingEnabled());

  // 2. Set up container and populate it with particles
  auto container = SimulationUtils::setupContainer(config.getContainerType());

  if (config.getInputType() == InputType::XML) {
    SimulationUtils::populateContainer(*container, config.xmlParser, config.loadCheckpoint());
  } else {
    SimulationUtils::populateContainerViaFile(*container, config.getInputPath().c_str(), config.getSimType());
  }

  // 3. Select model, writer, prepare thermostat
  auto model = SimulationUtils::getModel(Models::LennardJones);
  auto writer = config.isFileOutputEnabled() ? SimulationUtils::getWriter(Writers::NoWriter)
                                             : SimulationUtils::getWriter(Writers::VTKWriter);
  auto thermostat = SimulationUtils::getThermostat();

  // 4. Start simulation
  Simulation simulation{};
  simulation.setDeltaT(config.getDeltaT());
  simulation.setEndTime(config.getEndTime());
  simulation.setIterationsPerWrite(config.getWriteInterval());

  auto startTime = std::chrono::steady_clock::now();

  simulation.simulate(*model, *container, *writer, *thermostat, config.getGravityConst());

  auto endTime = std::chrono::steady_clock::now();

  // 5. Print performance stats
  if (config.isPerformanceMeasureEnabled()) ConfigurationUtils::printPerformanceMeasure(startTime, endTime, simulation);
  if (config.isMeasureHitrateEnabled()) ConfigurationUtils::printHitrateMeasure(std::move(model));

  spdlog::info("Terminating...");
  return EXIT_SUCCESS;
}