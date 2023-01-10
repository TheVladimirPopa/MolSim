
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

void configureLogging(int logLevel, bool disableLogging);

int main(int argc, char *argsv[]) {
  // Print help when no arguments or options are present
  if (argc == 1) {
    Configuration::printUsage();
    return 1;
  }

  // 1. Load config & configure logging (e.g. no logging when performance measurement)
  Configuration config = Configuration::parseOptions(argc, argsv);
  if (config.getInputType() == InputType::XML) SimulationUtils::parseXmlInput(config.getXmlPath());

  configureLogging(config.getLogLevel(), !config.hasLoggingEnabled());

  // 2. Set up container and populate it with particles
  auto container = SimulationUtils::setupContainer(config.getContainerType());

  if (config.getInputType() == InputType::XML) {
    SimulationUtils::populateContainer(*container, config.getInputType());
  } else {
    SimulationUtils::populateContainerFile(*container, config.getInPath());
  }

  // 3. Select model, writer, prepare thermostat
  auto model = SimulationUtils::getModel(Models::LennardJones);
  auto writer = config.hasFileOutputEnabled() ? SimulationUtils::getWriter(Writers::NoWriter)
                                              : SimulationUtils::getWriter(Writers::VTKWriter);
  auto thermostat = SimulationUtils::getThermostat();

  // 4. Start simulation
  Simulation simulation{};
  simulation.setDeltaT(config.getDeltaT());
  simulation.setEndTime(config.getEndTime());
  simulation.setIterationsPerWrite(config.getWriteInterval());

  auto startTime = std::chrono::steady_clock::now();

  simulation.simulate(*model,  *container, *writer, *thermostat, config.getGravityConst());

  // todo: printPerformance()
  // todo: printHitrateMeasurement()
  if (config.measurePerformance()) {
    auto endTime = std::chrono::steady_clock::now();
    auto durationSec = std::chrono::duration<double>{endTime - startTime}.count();
    auto iterationCount = std::ceil(simulation.getEndTime() / simulation.getDeltaT());
    std::cout << "Results of performance measurement\n"
                 "Execution time:              "
              << durationSec
              << "s\n"
                 "Number of iterations:        "
              << static_cast<unsigned long>(iterationCount) << "\n"
              << "Number of molecule updates:  " << simulation.getMoleculeUpdateCount()
              << "\n"
                 "Time per iteration:          "
              << (durationSec / iterationCount) << "s\n"
              << "Molecule Updates per second: "
              << (static_cast<double>(simulation.getMoleculeUpdateCount()) / durationSec) << std::endl;
  }
  if (config.measureHitrate()) {
    std::cout << "########################################################\n"
                 "Results of hit-rate measurement\n"
                 "Number of hits          : "
              << model->getHits()
              << "\n"
                 "Number of comparisons   : "
              << model->getComparisons()
              << "\n"
                 "Hit rate                : "
              << (static_cast<double>(model->getHits()) * 100. / static_cast<double>(model->getComparisons())) << "%"
              << std::endl;
  }

  spdlog::info("Terminating...");
  return 0;
}

void configureLogging(int logLevel, bool disableLogging) {
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("../logs/logger", 1048576 * 5, 5, true);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  if (disableLogging) logLevel = -1;

  switch (logLevel) {
    case -1:
      console_sink->set_level(spdlog::level::err);
      file_sink->set_level(spdlog::level::info);
      break;
    case 0:
      console_sink->set_level(spdlog::level::info);
      file_sink->set_level(spdlog::level::info);
      break;
    case 1:
      console_sink->set_level(spdlog::level::debug);
      file_sink->set_level(spdlog::level::debug);
      break;
    default:
      console_sink->set_level(spdlog::level::trace);
      file_sink->set_level(spdlog::level::trace);
      break;
  }

  spdlog::set_default_logger(std::make_shared<spdlog::logger>("", spdlog::sinks_init_list({console_sink, file_sink})));
  spdlog::set_level(spdlog::level::trace);
}
