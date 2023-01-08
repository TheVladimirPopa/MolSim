#pragma once

#include <string>
#include <map>
#include <string>

enum class InputType {
  File, XML
};

class Configuration {
 private:
  std::string outFileName;
  std::string inFileXml;
  std::string inFile;

  // Input, output
  bool noOutput{false}; // todo: disableFileWriting
  bool readCheckpoint{false};
  bool writeCheckpoint{false};
  int outputWriteInterval{1000};
  bool useXml;

  // Simulation parameters
  double deltaT{0.014};
  double endTime{1.0};

  InputType inputType{};

  // Performance measurement, log behavior
  bool performanceMeasure{false};
  bool hitRateMeasure{false};
  bool quietLog{false}; // disable console logging
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
  Configuration() {};
 public:
  // Todo: docs
  static Configuration parseOptions(int argc, char *argsv[]);
  static void printHelp();
  static void printUsage();

  double getLogLevel() { return logLevel; };
  bool isLogDisabled() { return quietLog; };
  ContainerType getContainerType() { /* TODO: return xml container type, linked or vector container */ }
  InputType getInputType() { return inputType; };
  std::string getXmlPath() { return inFileXml; };
};

