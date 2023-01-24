#include <chrono>
#include <cmath>

#include "utils/SimulationUtils.h"
#include "Configuration.h"
#include "Simulation.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"



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
  bool isXmlInput = config.getInputType() == InputType::XML;
  if (isXmlInput) {
    bool xmlInputOk = config.tryParseXml();

    if (!xmlInputOk) exit(EXIT_FAILURE);
  }

  ConfigurationUtils::configureLogging(config.getLogLevel(), !config.hasLoggingEnabled());

  // 2. Set up container and populate it with particles
  std::unique_ptr<IContainer> container;
  if (isXmlInput) {
    container = SimulationUtils::makeContainer(config.getContainerType(), config.takeContainer());
  } else {
    container = SimulationUtils::makeDefaultContainer();
  }

  if (isXmlInput) {
    SimulationUtils::populateContainer(*container, config.hasLoadCheckpointEnabled(), config.getParticleShapes());
  } else {
    SimulationUtils::populateContainerViaFile(*container, config.getInputPath(), config.getSimType());
  }

  if (config.hasLoadCheckpointEnabled()) SimulationUtils::loadCheckpoint(*container, config.getCheckpointPath());

  // 3. Select model, writer, prepare thermostat
  auto model = SimulationUtils::makeModel(config.getSelectedModel(), config.getDeltaT(), config.getCutOff());
  auto writer = config.hasFileOutputEnabled() ? SimulationUtils::makeWriter(WriterType::VTKWriter)
                                              : SimulationUtils::makeWriter(WriterType::NoWriter);
  auto thermostat = isXmlInput ? SimulationUtils::makeThermostat(config.takeThermostat())
                               : SimulationUtils::makeDefaultThermostat(*container);

  // 4. Start simulation
  Simulation simulation{};
  simulation.setDeltaT(config.getDeltaT());
  simulation.setEndTime(config.getEndTime());
  simulation.setIterationsPerWrite(config.getWriteInterval());

  auto startTime = std::chrono::steady_clock::now();

  simulation.simulate(*model, *container, *writer, *thermostat, config.getGravityConst(),
                      config.hasWriteCheckpointEnabled());

  auto endTime = std::chrono::steady_clock::now();

  // 5. Print performance stats
  if (config.hasPerformanceMeasureEnabled())
    ConfigurationUtils::printPerformanceMeasure(startTime, endTime, simulation);
  if (config.hasHitrateMeasureEnabled()) ConfigurationUtils::printHitrateMeasure(std::move(model));

  spdlog::info("Terminating...");
  return EXIT_SUCCESS;
}