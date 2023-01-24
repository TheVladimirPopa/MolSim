#pragma once

#include <map>
#include <string>
#include <variant>

#include "inputReader/XMLFileReader/XMLParser.h"
#include "utils/SimulationUtils.h"



class Configuration {
 private:
  std::string outFileName;
  std::string inFilePath;
  std::string checkpointPath;
  std::unique_ptr<XMLParser> xmlParser;
  std::vector<ParticleShape> particleShapes;

  // Input, output
  InputType inputType{InputType::XML};
  bool readCheckpoint{false};
  bool writeCheckpoint{false};
  bool noOutput{false};  // todo: disableFileWriting
  int outputWriteInterval{1000};

  // Simulation parameters
  double deltaT{0.014};
  double endTime{1.0};
  double gravityConstant{-12.44};
  double cutOff{4.0};
  ContainerType containerType{ContainerType::LINKED_CELLS};
  std::unique_ptr<LinkedCellsContainer> container;
  std::unique_ptr<Thermostat> thermostat;

  // Performance measurement, log behavior
  bool performanceMeasure{false};
  bool hitRateMeasure{false};
  bool quietLog{false};  // disable console logging
  int logLevel{0};

  // Converter functions for string input
  static double stringToEndTime(std::string endTimeAsString);
  static double stringToDeltaT(std::string deltaTAsString);
  static int getWriteInterval(std::string writeIntervalAsString);

  // Deprecated simulation type. XML now combines multiple types.
  SimTypeDeprecated simType{SimTypeDeprecated::Single};
  static SimTypeDeprecated stringToSimType(std::string name);

  // Use parseOptions to generate the Configuration object.
  Configuration() = default;
  ;

 public:
  // Todo: docs
  static Configuration parseOptions(int argc, char* argsv[]);
  static void printHelp();
  static void printUsage();
  bool tryParseXml();

  // TODO: Zeug renamen, vor allem bool getters
  double getLogLevel() { return logLevel; };
  bool hasLoadCheckpointEnabled() { return readCheckpoint; }
  bool hasWriteCheckpointEnabled() { return writeCheckpoint; }
  bool hasLoggingEnabled() { return !quietLog; };
  bool hasFileOutputEnabled() { return !noOutput; };
  bool hasPerformanceMeasureEnabled() { return performanceMeasure; };
  bool hasHitrateMeasureEnabled() { return hitRateMeasure; };

  ContainerType getContainerType() { return containerType; }
  InputType getInputType() const { return inputType; };

  std::string getInputPath() const {
    if (inputType == InputType::XML) return inFilePath;

    return inFilePath;
  }

  double getGravityConst() const { return gravityConstant; };
  double getDeltaT() const { return deltaT; };
  double getEndTime() const { return endTime; };
  double getWriteInterval() const { return outputWriteInterval; };
  double getCutOff() const { return cutOff; }

  std::vector<ParticleShape> getParticleShapes() { return particleShapes; }
  ModelType getSelectedModel() const {
    if (getInputType() != InputType::XML) return ModelType::LennardJones;

    return xmlParser->getModel();
  }

  std::string getCheckpointPath() const { return checkpointPath; };
  std::string getFileName() const { return outFileName; };
  SimTypeDeprecated getSimType() const { return simType; };
  std::unique_ptr<LinkedCellsContainer> takeContainer();
  std::unique_ptr<Thermostat> takeThermostat();
};
