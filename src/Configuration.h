#pragma once

#include <map>
#include <string>
#include <variant>

#include "inputReader/XMLFileReader/XMLParser.h"
#include "utils/SimulationUtils.h"

class Configuration {
 private:
  // File handling
  std::string outFileName;
  std::string inFilePath;
  std::string checkpointPath;
  std::vector<ParticleShape> particleShapes;
  std::unique_ptr<XMLParser> xmlParser;

  // Input, output
  InputType inputType{InputType::XML};
  bool readCheckpoint{false};
  bool writeCheckpoint{false};
  bool disableFileWriting{false};
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
  bool disableLogging{false};
  int logLevel{0};

  // Converter functions for string input
  static double stringToEndTime(std::string endTimeAsString);
  static double stringToDeltaT(std::string deltaTAsString);
  static int getWriteInterval(std::string writeIntervalAsString);

  // Deprecated simulation type. XML now combines multiple types.
  SimTypeDeprecated simType{SimTypeDeprecated::Single};
  static SimTypeDeprecated stringToSimType(std::string name);

  // Please use parseOptions(int, char*).
  Configuration() = default;

 public:
  // Parsing
  static Configuration parseOptions(int argc, char* argsv[]);
  bool tryParseXml();

  // Help messages
  static void printHelp();
  static void printUsage();

  // Boolean getters
  [[nodiscard]] bool hasLoggingEnabled() const { return !disableLogging; };
  [[nodiscard]] bool hasFileOutputEnabled() const { return !disableFileWriting; };
  [[nodiscard]] bool hasLoadCheckpointEnabled() const { return readCheckpoint; }
  [[nodiscard]] bool hasWriteCheckpointEnabled() const { return writeCheckpoint; }
  [[nodiscard]] bool hasPerformanceMeasureEnabled() const { return performanceMeasure; };
  [[nodiscard]] bool hasHitrateMeasureEnabled() const { return hitRateMeasure; };

  // Simulation getters
  [[nodiscard]] double getGravityConst() const { return gravityConstant; };
  [[nodiscard]] double getDeltaT() const { return deltaT; };
  [[nodiscard]] double getEndTime() const { return endTime; };
  [[nodiscard]] double getWriteInterval() const { return outputWriteInterval; };
  [[nodiscard]] double getCutOff() const { return cutOff; }
  [[nodiscard]] ModelType getSelectedModel() const {
    return getInputType() != InputType::XML ? ModelType::LennardJones : xmlParser->getModel();
  }
  [[nodiscard]] ContainerType getContainerType() { return containerType; }
  [[nodiscard]] SimTypeDeprecated getSimType() const { return simType; };
  [[nodiscard]] std::vector<ParticleShape> getParticleShapes() { return particleShapes; }

  [[nodiscard]] std::unique_ptr<LinkedCellsContainer> takeContainer();
  [[nodiscard]] std::unique_ptr<Thermostat> takeThermostat();

  // Input
  [[nodiscard]] std::string getInputPath() const { return inFilePath; }
  [[nodiscard]] InputType getInputType() const { return inputType; };
  [[nodiscard]] std::string getCheckpointPath() const { return checkpointPath; };

  // Output
  [[nodiscard]] std::string getFileName() const { return outFileName; };
  [[nodiscard]] double getLogLevel() const { return logLevel; };
};
