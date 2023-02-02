#include "SimulationUtils.h"

#include "dataStructures/VectorContainer.h"
#include "dataStructures/parallelizedLinkedCell/LinkedCellsContainerColouringMultiple.h"
#include "dataStructures/parallelizedLinkedCell/LinkedCellsContainerColouringSingle.h"
#include "dataStructures/parallelizedLinkedCell/LinkedCellsContainerLocks.h"
#include "model/SmoothedLennardJonesModel.h"
#include "outputWriter/StatisticsWriter.h"

std::unique_ptr<LinkedCellsContainer> SimulationUtils::makeDefaultContainer() {
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

std::unique_ptr<IContainer> SimulationUtils::makeContainer(ContainerType type, LinkedCellArg* spec) {
  if (type == ContainerType::VECTOR) return std::make_unique<VectorContainer>();

  // Currently only vector and linked cells containers are supported. Assume the container is linked cells.
  auto makeLinkedCells = [](auto type, double cellSize, auto left,
                            auto right) -> std::unique_ptr<LinkedCellsContainer> {
    if (type == ContainerType::LINKED_CELLS) return std::make_unique<LinkedCellsContainer>(cellSize, left, right);

    if (type == ContainerType::LINKED_CELLS_COLOURING_SINGLE)
      return std::make_unique<LinkedCellsContainerColouringSingle>(cellSize, left, right);

    if (type == ContainerType::LINKED_CELLS_COLOURING_MULTIPLE)
      return std::make_unique<LinkedCellsContainerColouringMultiple>(cellSize, left, right);

    if (type == ContainerType::LINKED_CELLS_LOCKS)
      return std::make_unique<LinkedCellsContainerLocks>(cellSize, left, right);

    throw std::runtime_error("Make function for ContainerType not implemented. Check SimulationUtils.cpp.");
  };

  if (spec == nullptr) throw std::runtime_error("Expected linked cells container specs. Check SimulationUtils.cpp.");
  auto container = makeLinkedCells(type, spec->getCellSize(), spec->getLeftLowerBound(), spec->getRightUpperBound());

  container->setBoundaries({
      {CubeSide::LEFT, XMLParser::strToEnumBoundary(spec->getBoundLeft())},
      {CubeSide::RIGHT, XMLParser::strToEnumBoundary(spec->getBoundRight())},
      {CubeSide::TOP, XMLParser::strToEnumBoundary(spec->getBoundTop())},
      {CubeSide::BOTTOM, XMLParser::strToEnumBoundary(spec->getBoundBottom())},
      {CubeSide::FRONT, XMLParser::strToEnumBoundary(spec->getBoundFront())},
      {CubeSide::BACK, XMLParser::strToEnumBoundary(spec->getBoundBack())},
  });

  return container;
}
void SimulationUtils::populateContainerViaFile(IContainer& container, std::string filePath, SimTypeDeprecated type) {
  spdlog::info("Generating particles via file input. (Warning: Deprecated)");
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
void SimulationUtils::populateContainer(IContainer& container, std::vector<ParticleShape> shapes) {
  spdlog::info("Generating particles.");
  for (auto shape : shapes) {
    if (std::holds_alternative<ParticleGeneration::cuboid>(shape))
      ParticleGeneration::addCuboidToParticleContainer(container, std::get<ParticleGeneration::cuboid>(shape));

    if (std::holds_alternative<ParticleGeneration::sphere>(shape))
      ParticleGeneration::addSphereToParticleContainer(container, std::get<ParticleGeneration::sphere>(shape));

    if (std::holds_alternative<ParticleGeneration::membrane>(shape))
      ParticleGeneration::addMembraneToParticleContainer(container, std::get<ParticleGeneration::membrane>(shape));
  }
}
void SimulationUtils::loadCheckpoint(IContainer& container, std::string checkpointPath) {
  spdlog::info("Loading checkpoint file.");
  FileReader::readFileCheckpoint(container, checkpointPath.data());
}
std::unique_ptr<IModel> SimulationUtils::makeModel(ModelType modelType, double deltaT, double cutOff, double radius_l) {
  std::unique_ptr<IModel> model;

  if (modelType == ModelType::LennardJones) {
    model = std::make_unique<LennardJonesModel>(cutOff);
  } else if (modelType == ModelType::SmoothedLennardJones) {
    model = std::make_unique<SmoothedLennardJonesModel>(cutOff, radius_l);
  } else if (modelType == ModelType::NewtonsLaw) {
    model = std::make_unique<NewtonsLawModel>();
  } else {
    throw std::runtime_error("If you add a new model, please update SimulationUtils::makeModel().");
  }

  model->setDeltaT(deltaT);
  return model;
}

std::unique_ptr<IWriter> SimulationUtils::makeWriter(WriterType writerType) {
  if (writerType == WriterType::VTKWriter) return std::make_unique<VTKWriter>();

  return std::make_unique<NoWriter>();
}

std::unique_ptr<Thermostat> SimulationUtils::makeDefaultThermostat(IContainer& container) {
  return std::make_unique<Thermostat>(container, 40., 40., 5., 1000, 2);
}

std::unique_ptr<Thermostat> SimulationUtils::makeThermostat(IContainer& container, ThermostatArg thermArg) {
  double maxDouble = 1.79769e+308;
  return std::make_unique<Thermostat>(container, thermArg.getInitialTemp(), thermArg.getTargetTemp(),
                                      thermArg.getMaxTempChange() < 0 ? maxDouble : thermArg.getMaxTempChange(),
                                      thermArg.getPeriodLength(), thermArg.getDimension());
}

void ConfigurationUtils::configureLogging(int logLevel, bool disableLogging) {
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

void ConfigurationUtils::printPerformanceMeasure(time_point startTime, time_point endTime, Simulation& simulation) {
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

void ConfigurationUtils::printHitrateMeasure(std::unique_ptr<IModel> model) {
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
