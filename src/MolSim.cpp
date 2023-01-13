
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

using duration = std::chrono::nanoseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

/**
 * Sets the global logging behaviour of the program.
 * @param logLevel Level 0: Print info and error messages. Level 1: Print debug and level 0 messages. Level 2: Print
 * trace and level 1 messages
 * @param disableLogging If true, only errors will be printed. Errors also normally cause the simulation to be
 * terminated.
 */
void configureLogging(int logLevel, bool disableLogging);

/**
 * Prints the measured performance of the simulation
 * @param startTime Time when simulation started. (Excludes loading of files)
 * @param endTime Time when simulation ended.
 * @param simulation The simulation object which stores the performance information.
 */
void printPerformanceMeasure(time_point startTime, time_point endTime, Simulation& simulation);

/**
 * Prints the hitrate of the model.
 * Hitrate is defined as (number_of_actual_calculations)/(number_of_total_calculations) where actual_calculations are
 * the calculations where two particles are within the cutoff radius. And total_calculations are calculations ignoring
 * the cutoff radius.
 * @param model The model that was used to perform the calculations.
 */
void printHitrateMeasure(std::unique_ptr<IModel> model);

// --------------------------------------
// |             Main                   |
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

  configureLogging(config.getLogLevel(), !config.isLoggingEnabled());

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
  if (config.isPerformanceMeasureEnabled()) printPerformanceMeasure(startTime, endTime, simulation);
  if (config.isMeasureHitrateEnabled()) printHitrateMeasure(std::move(model));

  spdlog::info("Terminating...");
  return EXIT_SUCCESS;
}

// --------------------------------------
// |        Main helper functions       |
// --------------------------------------
void configureLogging(int logLevel, bool disableLogging) {
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("../logs/logger", 1048576 * 5, 5, true);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  if (disableLogging) logLevel = -1;

  switch (logLevel) {
    case -1:
      console_sink->set_level(spdlog::level::err);
      file_sink->set_level(spdlog::level::err);
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

void printPerformanceMeasure(time_point startTime, time_point endTime, Simulation& simulation) {
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

void printHitrateMeasure(std::unique_ptr<IModel> model) {
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