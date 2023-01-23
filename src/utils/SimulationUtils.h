#pragma once

#include "Configuration.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "inputReader/XMLFileReader/XMLParser.h"
#include "outputWriter/NoWriter.h"
#include "spdlog/spdlog.h"
using duration = std::chrono::nanoseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;
using ParticleShape = std::variant<ParticleGeneration::sphere, ParticleGeneration::cuboid>;

enum class InputType { File, XML };
enum class ContainerType { VECTOR = 0, LINKED_CELLS = 1 };
enum class ModelType { NewtonsLaw, LennardJones };
enum class WriterType { NoWriter, VTKWriter };
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

std::unique_ptr<IContainer> makeContainer(ContainerType type, std::unique_ptr<IContainer> container) {
  if (type == ContainerType::VECTOR)
    return std::make_unique<VectorContainer>();
  else if (type == ContainerType::LINKED_CELLS)
    return container; // This is a placeholder for now
  else
    throw std::runtime_error("Unsupported container type. Check Simulationutils.h");
}

void populateContainerViaFile(IContainer &container, std::string filePath, SimTypeDeprecated type) {
  spdlog::info("Generating particles via File input. (Warning: Deprecated)");
  switch (type) {
    case SimTypeDeprecated::Single: {
      FileReader::readFileSingle(container, filePath.data());
      break;
    }
    case SimTypeDeprecated::Cuboid: {
      FileReader::readFileCuboid(container, filePath.data());
      break;
    }
    case SimTypeDeprecated::Sphere: {
      FileReader::readFileSphere(container, filePath.data());
      break;
    }
  }
}

void populateContainer(IContainer &container, bool loadCheckpoint, std::vector<ParticleShape> shapes) {
  spdlog::info("Generating particles via XML input.");
  for (auto shape : shapes) {
    if (std::holds_alternative<ParticleGeneration::cuboid>(shape))
      ParticleGeneration::addCuboidToParticleContainer(container, std::get<ParticleGeneration::cuboid>(shape));

    if (std::holds_alternative<ParticleGeneration::cuboid>(shape))
      ParticleGeneration::addSphereToParticleContainer(container, std::get<ParticleGeneration::sphere>(shape));

    // Todo: Add membrane
  }
}

void loadCheckpoint(IContainer &container, std::string checkpointPath) {
  spdlog::info("Loading checkpoint file.");
  FileReader::readFileCheckpoint(container, checkpointPath.data());
}

std::unique_ptr<IModel> makeModel(ModelType modelType, double deltaT, double cutOff = 1e9) {
  std::unique_ptr<IModel> model;

  if (modelType == ModelType::LennardJones) {
    model = std::make_unique<LennardJonesModel>(cutOff);
  } else if (modelType == ModelType::NewtonsLaw) {
    model = std::make_unique<NewtonsLawModel>();
  } else {
    throw std::runtime_error("If you add a new model, please update SimulationUtils::makeModel().");
  }

  model->setDeltaT(deltaT);
  return model;
}

std::unique_ptr<IWriter> makeWriter(WriterType writerType) {
  if (writerType == WriterType::VTKWriter) return std::make_unique<VTKWriter>();

  return std::make_unique<NoWriter>();
}

std::unique_ptr<Thermostat> makeDefaultThermostat(IContainer& container) {
  return std::make_unique<Thermostat>(container, 40., 40., 5., 1000, 2);
}

std::unique_ptr<Thermostat> makeThermostat(IContainer &container) {
  return parser.initialiseThermostatFromXML(container);
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
