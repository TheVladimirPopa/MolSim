
#include <getopt.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>

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
#include "Configuration.h"

void printHelp();
void printUsage();
enum simTypes { Single, Cuboid, Sphere };
const std::map<std::string, simTypes> simTypeStrings{
    {"single", simTypes::Single}, {"cuboid", simTypes::Cuboid}, {"sphere", simTypes::Sphere}};

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


  void configureLogging() {
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("../logs/logger", 1048576 * 5, 5, true);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    if (performanceMeasure) {
      noOutput = true;
    }
    if (quietLog) {
      console_sink->set_level(spdlog::level::err);
      file_sink->set_level(spdlog::level::info);
    } else {
      switch (loglevel) {
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
    }
    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("", spdlog::sinks_init_list({console_sink, file_sink})));
    spdlog::set_level(spdlog::level::trace);
  }

  void disableLogging() {
    console_sink->set_level(spdlog::level::off);
    file_sink->set_level(spdlog::level::off);
  }

  IContainer *setupContainer() {
    if (!xmlInput) return setupDeprecatedDefaultContainer();
    parser.initialiseSimulationFromXML(simulation);

    lennardJonesModel.setDeltaT(simulation.getDeltaT());

    if (parser.getContainerType() == XMLParser::ContainerType::LINKED_CELLS) return loadLinkedCellsContainer(parser);

    if (parser.getContainerType() == XMLParser::ContainerType::VECTOR) return loadVectorContainer(parser);

    throw std::invalid_argument("Unsupported container type. Please update MolSim.cpp");
  }

  IModel *setupModel() {
    lennardJonesModel.setDeltaT(simulation.getDeltaT());
    lennardJonesModel.setCutOffRadius(parser.getCutOffRadius())
  }

  VectorContainer *loadVectorContainer(XMLParser &) {
    VectorContainer v{};
    vectorContainer = v;
    return &(this->vectorContainer);
  }

  LinkedCellsContainer *loadLinkedCellsContainer(XMLParser &parser) {
    linkedCellsContainer = parser.initialiseLinkedCellContainerFromXML();

    // TODO:
  }

  void setupParticles(IContainer &container) {
    if (!xmlInput) {
      setupParticlesDeprecated(container);
      return;
    }

    parser.initializeParticleTypes();
    std::string checkpointFile = "./checkpoint.txt";
    parser.initCheckpoint(checkpointFile);

    parser.spawnSpheres(container);
    parser.spawnCuboids(container);

    if (readFromCheckpoint) FileReader::readFileCheckpoint(container, checkpointFile.data());
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

int main(int argc, char *argsv[]) {
  // Print help when no arguments or options are present
  if (argc == 1) {
    printUsage();
    return 1;
  }

  // 1. Set up logging
  env.configureLogging();

  // 2. Load config
  Configuration config = Configuration::parseOptions(argc, argsv);

  // 3. Read input, generate particles

  IContainer *container = env.setupContainer();
  IModel *model = env.setupModel();
  env.setupParticles(*container);
  IWriter *selectedWriter = env.getSelectedWriter();
  Thermostat thermostat = env.getThermostat();
  double const gravityConstant = env.xmlInput ? env.parser.getGravityConstant() : -12.44;

  // 4. Start simulation
  Simulation simulation{};
  simulation.setDeltaT(config.deltaT);
  simulation.setEndTime(config.endTime);
  simulation.setIterationsPerWrite(confige.outputWriteInterval);

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


