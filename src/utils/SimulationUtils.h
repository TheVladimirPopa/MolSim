#pragma once

#include "inputReader/XMLFileReader/XMLParser.h"
using ContainerType = XMLParser::ContainerType;

#include "Configuration.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "spdlog/spdlog.h"
using duration = std::chrono::nanoseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

enum class Models { NewtonsLaw, LennardJones };

enum class Writers { NoWriter, VTKWriter };

enum class SimTypeDeprecated { Single, Cuboid, Sphere };

namespace SimulationUtils {

std::unique_ptr<LinkedCellsContainer> makeDefaultContainer() {
  // Fallback, when no linked cells container is specified.
  std::array<double, 3> leftLowerCorner{-15., -20., -5};
  std::array<double, 3> rightUpperCorner{55., 30., 5};

  auto defaultContainer = std::make_unique<LinkedCellsContainer>(10., leftLowerCorner, rightUpperCorner);

  defaultContainer->setBoundaries({
      {CubeSide::LEFT, BoundaryType::PERIODIC},
      {CubeSide::RIGHT, BoundaryType::PERIODIC},
      {CubeSide::TOP, BoundaryType::REFLECT},
      {CubeSide::BOTTOM, BoundaryType::REFLECT},
      {CubeSide::FRONT, BoundaryType::PERIODIC},
      {CubeSide::BACK, BoundaryType::PERIODIC},
  });

  return defaultContainer;
}

std::unique_ptr<IContainer> setupContainer(ContainerType containerType) {
  if (containerType == ContainerType::VECTOR) return std::make_unique<VectorContainer>();

  // Todo: linked_cells container is exclusively possible with xml input
  if (containerType == ContainerType::LINKED_CELLS) {
    if (!xmlParser) {
      spdlog::info("You are using file input. The structure of your linked cells container is undefined.");
      spdlog::info("Creating fallback container with hard coded values.");
      return makeDefaultContainer();
    }

    // Todo: Die Reference die wir vom XML Parser bekommen ist ein LinkedCellsContainer_t. Warum?
    std::unique_ptr<LinkedCellsContainer> linkedCellsContainer = xmlParser->initialiseLinkedCellContainerFromXML();

    return linkedCellsContainer;
  }

  throw std::invalid_argument("Unsupported container type. Please update MolSim.cpp");
}

void populateContainerViaFile(IContainer &container, char *filePath, SimTypeDeprecated type) {
  switch (type) {
    case SimTypeDeprecated::Single: {
      FileReader::readFileSingle(container, filePath);
      break;
    }
    case SimTypeDeprecated::Cuboid: {
      FileReader::readFileCuboid(container, filePath);
      break;
    }
    case SimTypeDeprecated::Sphere: {
      FileReader::readFileSphere(container, filePath);
      break;
    }
  }
}

void populateContainer(IContainer &container, bool loadCheckpoint, std::vector<ParticleShape> shapes) {
  parser.initializeParticleTypes();
  std::string checkpointFile = parser.getCheckpointPath();

  for (auto shape : shapes) {
    if (std::holds_alternative<ParticleGeneration::cuboid>(shape))
      ParticleGeneration::addCuboidToParticleContainer(container, std::get<ParticleGeneration::cuboid>(shape));

    if (std::holds_alternative<ParticleGeneration::cuboid>(shape))
      ParticleGeneration::addSphereToParticleContainer(container, std::get<ParticleGeneration::sphere>(shape));
  }

  if (loadCheckpoint) FileReader::readFileCheckpoint(container, checkpointFile.data());
}

std::unique_ptr<IModel> getModel(Models modelType) {
  // Todo: This is currently always the default lennard jones model
  if (modelType == Models::LennardJones) {
    auto lennardJones = std::make_unique<LennardJonesModel>(3.0);

    // todo: wie bringen wir das sauber hier rein?
    lennardJones->setDeltaT(simulation.getDeltaT());
    lennardJones->setCutOffRadius(parser.getCutOffRadius());

    return lennardJones;
  }

  return std::make_unique<NewtonsLawModel>();
}

std::unique_ptr<IWriter> getWriter(Writers writerType) {
  if (writerType == Writers::VTKWriter) return std::make_unique<VTKWriter>();

  return std::make_unique<NoWriter>();
}

std::unique_ptr<Thermostat> getThermostat() {
  if (!xmlInput) {
    return Thermostat{*getSelectedContainer(), 40., 40., 5., 1000, 2};
  }

  return parser.initialiseThermostatFromXML(*getSelectedContainer());
}

}  // namespace SimulationUtils

namespace ConfigurationUtils {

/**
 * Sets the global logging behaviour of the program.
 * @param logLevel Level 0: Print info and error messages. Level 1: Print debug and level 0 messages. Level 2: Print
 * trace and level 1 messages
 * @param disableLogging If true, only errors will be printed. Errors also normally cause the simulation to be
 * terminated.
 */
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

/**
 * Prints the measured performance of the simulation
 * @param startTime Time when simulation started. (Excludes loading of files)
 * @param endTime Time when simulation ended.
 * @param simulation The simulation object which stores the performance information.
 */
void printPerformanceMeasure(time_point startTime, time_point endTime, Simulation &simulation) {
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

/**
 * Prints the hitrate of the model.
 * Hitrate is defined as (number_of_actual_calculations)/(number_of_total_calculations) where actual_calculations are
 * the calculations where two particles are within the cutoff radius. And total_calculations are calculations ignoring
 * the cutoff radius.
 * @param model The model that was used to perform the calculations.
 */
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
}  // namespace ConfigurationUtils
