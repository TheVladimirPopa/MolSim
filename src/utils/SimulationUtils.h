#pragma once

#include "inputReader/XMLFileReader/XMLParser.h"
using ContainerType = XMLParser::ContainerType;
#include "Configuration.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "spdlog/spdlog.h"

enum class Models { NewtonsLaw, LennardJones };

enum class Writers { NoWriter, VTKWriter };

namespace SimulationUtils {
std::unique_ptr<XMLParser> xmlParser;

void parseXmlInput(std::string xmlPath) {
  // The constructor of the parser already does the parsing
  xmlParser = std::make_unique<XMLParser>(xmlPath);

  // TODO! This probably should be part of the configuration class!
}

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

void populateContainerFile(IContainer &container, char *filePath) {
  switch (simulationType) {
    case simTypes::Single: {
      FileReader::readFileSingle(container, filePath);
      break;
    }
    case simTypes::Cuboid: {
      FileReader::readFileCuboid(container, filePath);
      break;
    }
    case simTypes::Sphere: {
      FileReader::readFileSphere(container, filePath);
      break;
    }
  }
}

void populateContainer(IContainer &container) {
  parser.initializeParticleTypes();
  std::string checkpointFile = "./checkpoint.txt";
  parser.initCheckpoint(checkpointFile);

  parser.spawnSpheres(container);
  parser.spawnCuboids(container);

  if (readFromCheckpoint) FileReader::readFileCheckpoint(container, checkpointFile.data());
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
