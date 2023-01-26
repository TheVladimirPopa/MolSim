#pragma once

#include <map>
#include <string>
#include <variant>

#include "inputReader/XMLFileReader/XMLParser.h"
#include "utils/SimulationUtils.h"

class Configuration {
 private:
  // File handling
  /// The file name the output files will have
  std::string outFileName;

  /// Path from where to read input (either file input or xml input file)
  std::string inFilePath;

  /// Path from where to read checkpoint data from
  std::string checkpointPath;

  /// The XML parser
  std::unique_ptr<XMLParser> xmlParser;

  /// The shapes read from the xml input (e.g. cuboids, membranes)
  std::vector<ParticleShape> particleShapes;

  // Input, output
  /// Type of input (either file or xml)
  InputType inputType{InputType::XML};

  /// Whether or not to read the checkpoint file
  bool readCheckpoint{false};

  /// Whether or not to write a checkpoint file after the simulation is finished
  bool writeCheckpoint{false};

  /// Whether or not to write files
  bool disableFileWriting{false};

  /// The number of iterations after which a output file is created
  int outputWriteInterval{1000};

  // Simulation parameters
  /// Duration of a single simulated timestep
  double deltaT{0.014};

  /// The time at which the simulation ends
  double endTime{1.0};

  /// The gravity that is applied onto the simulation
  double gravityConstant{-12.44};

  /// The cutoff distance after which for a particle pair force calculation is ignored
  double cutOff{4.0};

  /// The type of the container where particles are stored (e.g. vector or linked cells)
  ContainerType containerType{ContainerType::LINKED_CELLS};

  /// The container itself
  std::unique_ptr<LinkedCellsContainer> container;

  /// The thermostat of the container
  std::unique_ptr<ThermostatArg> thermostat;

  // Performance measurement, log behavior
  /// Enable performance logging
  bool performanceMeasure{false};

  /// Enable hitrate measuring
  bool hitRateMeasure{false};

  /// Disable all log output
  bool disableLogging{false};

  /// The selected loglevel. The higher the log level the more information with be displayed.
  int logLevel{0};

  // Converter functions for string input
  /// Convert string to double and check whether it's a sensible simulation end-time
  static double stringToEndTime(std::string endTimeAsString);

  /// Convert string to double and check whether it's a sensible simulation delta_t
  static double stringToDeltaT(std::string deltaTAsString);

  /// Convert string to double and check whether it's a sensible write interval
  static int getWriteInterval(std::string writeIntervalAsString);

  // Deprecated simulation type. XML now combines multiple types.
  /// If file input whether the input file contains individual particles, cuboids or spheres
  SimTypeDeprecated simType{SimTypeDeprecated::Single};

  /// Convert string to simulation type
  static SimTypeDeprecated stringToSimType(std::string name);

  // The configuration object is meant to be created via Configuration::parseOptions(int, char*).
  Configuration() = default;

 public:
  // Parsing
  /**
   * Parse input parameters from command line.
   * @param argc Number of arguments
   * @param argsv The arguments
   * @return The parsed configuration object that contains what was read from command line.
   */
  static Configuration parseOptions(int argc, char* argsv[]);

  /**
   * Parses the xml file that was defined via -i in console. The function updates the configuration object with the
   * information of the xml file.
   * @return true if the file was parsed successfully, false otherwise
   */
  bool tryParseXml();

  // Help messages
  /**
   * Prints a verbose message on how to use the command line interface
   */
  static void printHelp();

  /**
   * Prints a short message on how to use the command line interface
   */
  static void printUsage();

  // Boolean getters for log options
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
  [[nodiscard]] ThermostatArg* getThermostat() { return thermostat.get(); };

  // Input
  [[nodiscard]] std::string getInputPath() const { return inFilePath; }
  [[nodiscard]] InputType getInputType() const { return inputType; };
  [[nodiscard]] std::string getCheckpointPath() const { return checkpointPath; };

  // Output
  [[nodiscard]] std::string getFileName() const { return outFileName; };
  [[nodiscard]] double getLogLevel() const { return logLevel; };
};
