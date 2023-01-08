#pragma once

#include "inputReader/XMLFileReader/XMLParser.h"
using ContainerType = XMLParser::ContainerType;
#include "Configuration.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "spdlog/spdlog.h"

namespace SimulationUtils {
std::unique_ptr<XMLParser> xmlParser;

void parseXmlInput(std::string xmlPath) {
  // The constructor of the parser already does the parsing
  xmlParser = std::make_unique<XMLParser>(xmlPath);

  // TODO! This probably should be part of the configuration class!
}

std::unique_ptr<IContainer> setupContainer(ContainerType containerType) {
  if (containerType == ContainerType::VECTOR) return std::make_unique<VectorContainer>();

  // Todo: linked_cells container is exclusively possible with xml input
  if (containerType == ContainerType::LINKED_CELLS) {
    if (!xmlParser) {
      spdlog::error("The linked cells container is only supported when using xml input. Please check --help.");
      exit(EXIT_FAILURE);
    }

    // Todo: Die Reference die wir vom XML Parser bekommen ist ein LinkedCellsContainer_t. Warum?
    std::unique_ptr<LinkedCellsContainer> linkedCellsContainer = xmlParser->initialiseLinkedCellContainerFromXML();

    return linkedCellsContainer;
  }

  throw std::invalid_argument("Unsupported container type. Please update MolSim.cpp");
}

void setupParticles(IContainer &container, InputType inputType) {
  if (inputType == InputType::File) {
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

}  // namespace SimulationUtils