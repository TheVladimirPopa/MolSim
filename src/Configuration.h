#pragma once

#include <map>
#include <string>
#include <variant>

#include "inputReader/XMLFileReader/XMLParser.h"

using ParticleShape = std::variant<ParticleGeneration::sphere, ParticleGeneration::cuboid>;

enum class InputType { File, XML };

class Configuration {
 private:
  std::string outFileName;
  std::string inFileXml;
  std::string inFile;  // Todo: variable sinnvoll umbenennen
  std::unique_ptr<XMLParser> xmlParser;
  std::vector<ParticleTypes> particleTypes; // TODO: Implementieren
  std::vector<ParticleShape> particleShapes;
  std::vector<Particle> checkPointData; // TODO: 1. implementieren, 2: vorsicht, das kann sehr groß sein!

  // Input, output
  bool noOutput{false};  // todo: disableFileWriting
  bool readCheckpoint{false};
  bool writeCheckpoint{false};
  int outputWriteInterval{1000};

  // Simulation parameters
  double deltaT{0.014};
  double endTime{1.0};
  InputType inputType{};
  double gravityConstant{-12.44};

  // Performance measurement, log behavior
  bool performanceMeasure{false};
  bool hitRateMeasure{false};
  bool quietLog{false};  // disable console logging
  int logLevel{0};

  // Converter functions for string input
  static double getEndTime(std::string endTimeAsString);
  static double getDeltaT(std::string deltaTAsString);
  static int getWriteInterval(std::string writeIntervalAsString);

  // Deprecated simulation type. XML now combines multiple types.
  enum class simulationType { Single, Cuboid, Sphere };
  simulationType simType{simulationType::Single};
  static enum simulationType getSimulationTypeDeprecated(std::string name);

  // Use parseOptions to generate the Configuration object.
  Configuration(){};

 public:
  // Todo: docs
  static Configuration parseOptions(int argc, char* argsv[]);
  static void printHelp();
  static void printUsage();
  bool tryParseXml();

  // TODO: Zeug renamen, vor allem bool getters
  double getLogLevel() { return logLevel; };
  bool isLoadCheckpointEnabled() { return readCheckpoint; }
  bool isWriteCheckpointEnabled() { return writeCheckpoint; }
  bool isLoggingEnabled() { return !quietLog; };
  bool isFileOutputEnabled() { return !noOutput; };
  bool isPerformanceMeasureEnabled() { return performanceMeasure; };
  bool isMeasureHitrateEnabled() { return hitRateMeasure; };

  ContainerType getContainerType() { /* TODO: return xml container type, linked or vector container */
  }
  InputType getInputType() { return inputType; };

  std::string getInputPath() const {
    if (inputType == InputType::XML) return inFileXml;

    return inFile;
  }

  double getGravityConst() const { return gravityConstant; };
  double getDeltaT() const { return deltaT; };
  double getEndTime() const { return endTime; };
  double getWriteInterval() const { return outputWriteInterval; };
  std::vector<ParticleShape> getParticleShapes() { return particleShapes; }
};
