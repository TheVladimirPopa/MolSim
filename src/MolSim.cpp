
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

struct Environment {
  char *inputFile = nullptr;

  XMLParser parser = XMLParser(xmlPath);

  // Selectable models
  NewtonsLawModel newtonsLawModel{};
  LennardJonesModel lennardJonesModel{3.0};

  // Selectable container types
  VectorContainer vectorContainer{};
  LinkedCellsContainer linkedCellsContainer{1, {0., 0., 0.}, {1., 1., 1.}};

  // Selectable writers
  VTKWriter vtkWriter{};
  NoWriter noWriter{};

  void disableLogging() {
    console_sink->set_level(spdlog::level::off);
    file_sink->set_level(spdlog::level::off);
  }

  IContainer *setupContainer() {

  }

  IModel *setupModel() {
    lennardJonesModel.setDeltaT(simulation.getDeltaT());
    lennardJonesModel.setCutOffRadius(parser.getCutOffRadius())
  }







  IContainer *getSelectedContainer() {
    if (parser.getContainerType() == XMLParser::ContainerType::LINKED_CELLS) return &linkedCellsContainer;

    if (parser.getContainerType() == XMLParser::ContainerType::VECTOR) return &vectorContainer;

    throw std::invalid_argument("Unsupported container type. Please update MolSim.cpp");
  }

  void setupParticlesDeprecated(IContainer &container) {
    // Todo
  }

  LinkedCellsContainer *setupDeprecatedDefaultContainer() {
    std::array<double, 3> leftLowerCorner{-15., -20., -5};
    std::array<double, 3> rightUpperCorner{55., 30., 5};

    LinkedCellsContainer testContainer{10., leftLowerCorner, rightUpperCorner};
    linkedCellsContainer = testContainer;

    linkedCellsContainer.setBoundaries({
        {CubeSide::LEFT, BoundaryType::PERIODIC},
        {CubeSide::RIGHT, BoundaryType::PERIODIC},
        {CubeSide::TOP, BoundaryType::REFLECT},
        {CubeSide::BOTTOM, BoundaryType::REFLECT},
        {CubeSide::FRONT, BoundaryType::PERIODIC},
        {CubeSide::BACK, BoundaryType::PERIODIC},
    });

    switch (simulationType) {
      case simTypes::Single: {
        FileReader::readFileSingle(linkedCellsContainer, inputFile);
        break;
      }
      case simTypes::Cuboid: {
        FileReader::readFileCuboid(linkedCellsContainer, inputFile);
        break;
      }
      case simTypes::Sphere: {
        FileReader::readFileSphere(linkedCellsContainer, inputFile);
        break;
      }
    }

    return &linkedCellsContainer;
  }

  IWriter *getSelectedWriter() {
    if (noOutput) {
      spdlog::info("No output will be written.");
      return &noWriter;
    } else {
      return &vtkWriter;
    }
  }

  Thermostat getThermostat() {
    if (!xmlInput) {
      return Thermostat{*getSelectedContainer(), 40., 40., 5., 1000, 2};
    }

    return parser.initialiseThermostatFromXML(*getSelectedContainer());
  }
};

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

int main(int argc, char *argsv[]) {
  // Print help when no arguments or options are present
  if (argc == 1) {
    Configuration::printUsage();
    return 1;
  }

  // 1. Load config
  Configuration config = Configuration::parseOptions(argc, argsv);
  if (config.getInputType() == InputType::XML) SimulationUtils::parseXmlInput(config.getXmlPath());

  // 2. Set up logging
  configureLogging(config.getLogLevel(), config.isLogDisabled());

  // 3. Set up container and populate it with particles

  auto container = SimulationUtils::setupContainer(config.getContainerType());
  SimulationUtils::populateContainer(*container, config.getInputType())

  env.setupParticles(*container);
  IModel *model = env.setupModel();
  IWriter *selectedWriter = env.getSelectedWriter();
  Thermostat thermostat = env.getThermostat();
  double const gravityConstant = env.xmlInput ? env.parser.getGravityConstant() : -12.44;

  // 4. Start simulation
  Simulation simulation{};
  simulation.setDeltaT(config.deltaT);
  simulation.setEndTime(config.endTime);
  simulation.setIterationsPerWrite(confige.outputWriteInterval);

  if (performanceMeasure) {
    noOutput = true;
  }

  if (env.performanceMeasure) {
    spdlog::info(
        "Setup done! Deactivating logging and starting performance measurement "
        "now...");

    env.disableLogging();
  }

  auto startTime = std::chrono::steady_clock::now();

  env.simulation.simulate(*model, *container, *selectedWriter, thermostat, -12.44);

  if (env.performanceMeasure) {
    auto endTime = std::chrono::steady_clock::now();
    auto durationSec = std::chrono::duration<double>{endTime - startTime}.count();
    auto iterationCount = std::ceil(env.simulation.getEndTime() / env.simulation.getDeltaT());
    std::cout << "Results of performance measurement\n"
                 "Execution time:              "
              << durationSec
              << "s\n"
                 "Number of iterations:        "
              << static_cast<unsigned long>(iterationCount) << "\n"
              << "Number of molecule updates:  " << env.simulation.getMoleculeUpdateCount()
              << "\n"
                 "Time per iteration:          "
              << (durationSec / iterationCount) << "s\n"
              << "Molecule Updates per second: "
              << (static_cast<double>(env.simulation.getMoleculeUpdateCount()) / durationSec) << std::endl;
  }
  if (env.hitRateMeasure) {
    std::cout << "########################################################\n"
                 "Results of hit-rate measurement\n"
                 "Number of hits          : "
              << model.getHits()
              << "\n"
                 "Number of comparisons   : "
              << model.getComparisons()
              << "\n"
                 "Hit rate                : "
              << (static_cast<double>(model.getHits()) * 100. / static_cast<double>(model.getComparisons())) << "%"
              << std::endl;
  }

  spdlog::info("Terminating...");
  return 0;
}
