#include "Configuration.h"

#include <getopt.h>

#include <iostream>

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using std::string;

enum SimTypeDeprecated Configuration::stringToSimType(string name) {
  const std::map<std::string, SimTypeDeprecated> simTypeStrings{{"single", SimTypeDeprecated::Single},
                                                                {"cuboid", SimTypeDeprecated::Cuboid},
                                                                {"sphere", SimTypeDeprecated::Sphere}};

  auto result = simTypeStrings.find(name);
  if (result == simTypeStrings.end()) {
    spdlog::error("Type ", optarg, " is not known. Please check --help.");
    exit(EXIT_FAILURE);
  }

  return result->second;
}

double Configuration::stringToEndTime(string endTimeAsString) {
  double end{0.0};

  try {
    end = std::stod(endTimeAsString);
  } catch (std::invalid_argument &e) {
    spdlog::error("End-time is not in a supported format: ", endTimeAsString, ". Please check --help.");
    exit(EXIT_FAILURE);
  }

  if (end <= 0) {
    spdlog::error("End-time has to be positive, but it is ", end, " . Please check --help.");
    exit(EXIT_FAILURE);
  }

  return end;
}

double Configuration::stringToDeltaT(string deltaTAsString) {
  double delta{0.0};

  try {
    delta = std::stod(deltaTAsString);
  } catch (std::invalid_argument &e) {
    spdlog::error("DeltaT has to be double, but failed to convert: ", deltaTAsString, " . Please check --help.");
    exit(EXIT_FAILURE);
  }

  if (delta <= 0) {
    spdlog::error("DeltaT has to be positive but is: ", delta, " . Please check --help.");
    exit(EXIT_FAILURE);
  }

  return delta;
}

int Configuration::getWriteInterval(string writeIntervalAsString) {
  int frequency{0};

  try {
    frequency = std::stoi(writeIntervalAsString);
  } catch (std::invalid_argument &e) {
    spdlog::error("WriteOutFrequency is not in a supported format: ", writeIntervalAsString, ". Please check --help.");
    exit(EXIT_FAILURE);
  }

  if (frequency <= 0) {
    spdlog::error("WriteOutFrequency has to be positive integer but ", frequency, "is negative. Please check --help.");
    exit(EXIT_FAILURE);
  }

  return frequency;
}

Configuration Configuration::parseOptions(int argc, char *argsv[]) {
  // Specify command line input parameters
  static struct option long_options[] = {{"xml", required_argument, nullptr, 'i'},
                                         {"export-checkpoint", required_argument, nullptr, 's'},
                                         {"input-checkpoint", no_argument, nullptr, 'c'},
                                         {"output-file", required_argument, nullptr, 'o'},
                                         {"no-output", no_argument, nullptr, 'n'},
                                         {"statistics", no_argument, nullptr, 'j'},
                                         {"type", required_argument, nullptr, 't'},
                                         {"input-file", required_argument, nullptr, 'f'},
                                         {"end-time", required_argument, nullptr, 'e'},
                                         {"delta-t", required_argument, nullptr, 'd'},
                                         {"write-frequency", required_argument, nullptr, 'w'},
                                         {"performance", no_argument, nullptr, 'p'},
                                         {"hit-rate", no_argument, nullptr, 'r'},
                                         {"verbose", no_argument, nullptr, 'v'},
                                         {"quiet", no_argument, nullptr, 'q'},
                                         {"help", no_argument, nullptr, 'h'},
                                         {nullptr, 0, nullptr, 0}};

  // We generate configuration object with default values and then overwrite the defaults.
  Configuration config{};

  // Define behavior
  int opt{0};
  while ((opt = getopt_long(argc, argsv, "s:ci:o:njt:f:e:d:w:prvqh", long_options, nullptr)) != -1) {
    switch (opt) {
      case 'o': {
        config.outFileName = optarg;
        break;
      }
      case 'n': {
        config.disableFileWriting = true;
        break;
      }
      case 't': {
        config.simType = stringToSimType(string(optarg));
        break;
      }
      case 'f': {
        if (!config.inFilePath.empty()) spdlog::error("Make sure not to combine -i and -x.");

        config.inFilePath = optarg;
        config.inputType = InputType::File;
        break;
      }
      case 'i': {
        if (!config.inFilePath.empty()) spdlog::error("Make sure not to combine -i and -x.");

        config.inFilePath = optarg;
        config.inputType = InputType::XML;
        break;
      }
      case 'c': {
        config.readCheckpoint = true;
        break;
      }
      case 'j': {
        spdlog::info("Statistics will be printed out.");
        config.registerStatistics = true;
        break;
      }
      case 'e': {
        config.endTime = stringToEndTime(string(optarg));
        break;
      }
      case 'd': {
        config.deltaT = stringToDeltaT((string(optarg)));
        break;
      }
      case 's': {
        config.toCheckpointPath = optarg != nullptr ? optarg : "./checkpoint.txt";
        if (optarg == nullptr) spdlog::info("No checkpoint output file was specified, so ./checkpoint.txt is used");
        config.writeCheckpoint = true;
        break;
      }
      case 'w': {
        config.outputWriteInterval = getWriteInterval(string(optarg));
        break;
      }
      case 'p': {
        config.performanceMeasure = true;
        break;
      }
      case 'r': {
        config.hitRateMeasure = true;
        break;
      }
      case 'v': {
        config.logLevel += 1;
        break;
      }
      case 'q': {
        config.disableLogging = true;
        break;
      }
      case 'h': {
        printHelp();
        exit(EXIT_SUCCESS);
      }
      case '?':
      default: {
        printUsage();
        exit(EXIT_FAILURE);
      }
    }
  }

  if (config.inFilePath.empty()) {
    spdlog::error("You must specify an input file with -i (xml, recommended) or -f (file). For details check --help.");
    exit(EXIT_FAILURE);
  }

  if (config.hasPerformanceMeasureEnabled() && config.hasLoggingEnabled()) {
    spdlog::info("Disabling file output and logging for performance measurement.");
    config.disableLogging = true;
    config.disableFileWriting = true;
  }

  return config;
}

bool Configuration::tryParseXml() {
  if (inFilePath.empty() || inputType != InputType::XML) return false;

  try {
    xmlParser = std::make_unique<XMLParser>(inFilePath);
  } catch (const std::exception &exception) {
    spdlog::error("Failed to load XML. Error: {}", std::string(exception.what()));
    return false;
  }

  endTime = xmlParser->getEndTime();
  deltaT = xmlParser->getDeltaT();
  gravityConstant = xmlParser->getGravityConstant();
  outputWriteInterval = xmlParser->getWriteInterval();
  cutOff = xmlParser->getCutOffRadius();
  radius_l = xmlParser->getRadius_l();
  containerType = xmlParser->getContainerType();
  outFileName = xmlParser->extractSimulation().getFilename();

  for (auto sphere : xmlParser->getSpheres()) particleShapes.push_back(sphere);
  for (auto cuboid : xmlParser->getCuboids()) particleShapes.push_back(cuboid);
  for (auto membrane : xmlParser->getMembranes()) particleShapes.push_back(membrane);

  xmlParser->initializeParticleTypes();
  fromCheckpointPath = xmlParser->getCheckpointPath();
  container = xmlParser->initialiseLinkedCellContainerFromXML();
  thermostat = std::make_unique<ThermostatArg>(xmlParser->getThermostat());

  StatArg statArg = xmlParser->extractStatistics();
  statFrequency = statArg.getFrequency();
  rdfDeltaR = statArg.getRdfDeltaR();
  rdfStart = statArg.getRdfStart();
  rdfEnd = statArg.getRdfEnd();
  statFile = statArg.getPath();

  return true;
}

std::unique_ptr<LinkedCellsContainer> Configuration::takeContainer() { return std::move(container); }

void Configuration::printUsage() {
  std::cout << " Usage\n"
               "        ./MolSim -i <input-file> [-n] [-p] [-r] [-s] [-c] [-j] [-v] [-v] [-q] [-x]\n"
               "\n"
               " Usage (deprecated)\n"
               "        ./MolSim -f <input-file> [-t (single|cuboid|sphere)] [-o "
               "<output-file>] [-e <endtime>]\n"
               "                                [-d <deltaT>] [-w <iteration-count>] "
               "[-n] [-p] [-r] [-s] [-c] [-j] [-v] [-v] [-q] [-x]\n"
               "\n"
               "For more information run ./Molsim -h or ./Molsim --help"
            << std::endl;
}

void Configuration::printHelp() {
  std::cout << "Usage\n"
               "        ./MolSim -f <input-file> [-t (single|cuboid|sphere)] [-o "
               "<output-file>] [-e <endtime>]\n"
               "                                [-d <deltaT>] [-w <iteration-count>] "
               "[-n] [-j] [-p] [-r] [-v] [-v] [-q]\n"
               "\n"
               "OPTIONS:\n"
               "        -i <filepath>, --xml=<filepath>\n"
               "                Use the given <filepath> of an XML file as an input for the simulation.\n"
               "        \n"
               "        -s, --export-checkpoint\n"
               "                When set, the state of the particles after the simulation are saved to\n"
               "                the file ./checkpoint.txt\n"
               "                \n"
               "        -c, --input-checkpoint\n"
               "                When set, a previous checkpoint is taken as an input, in addition to the \n"
               "                XML input\n"
               "\n"
               "        -o <filepath>, --output-name=<filepath>\n"
               "                Use the given <filepath> as the path for \n"
               "                the outputfiles(iteration number and file-ending are "
               "added automatically)\n"
               "                If not specified \"MD_vtk\" is used\n"
               "                \n"
               "        -j, --statistics\n"
               "                If active particle statistics (e.g. the radial distribution function) will \n"
               "                be printed out\n"
               "        -n, --no-output\n"
               "                If active no files will be written, even overwrites "
               "-o.\n"
               "\n"
               "        -t (single|cuboid|sphere), --type=(single|cuboid|sphere)\n"
               "                Specifies the way to set up particles (default is "
               "single).\n"
               "                Use single if you want particles on their own and "
               "use \n"
               "                cuboid if you want the particles to spawn in "
               "cuboids.\n"
               "\n"
               "        -f <filepath>, --input-file=<filepath>\n"
               "                Use the file at the <filepath> as an input.\n"
               "\n"
               "        -e <endtime>, --end-time=<endtime>\n"
               "                The time until the simulation is run (default is "
               "1000).\n"
               "\n"
               "        -d <deltaT>, --delta-t=<deltaT>\n"
               "                The timestep by which the time gets increased \n"
               "                in every iteration (default is 0.014).\n"
               "\n"
               "        -w <iteration-count>, --write-frequency=<iteration-count>\n"
               "                Every <iteration-count>nth iteration the particles \n"
               "                get written to a file (default is 10).\n"
               "        \n"
               "        -p, --performance\n"
               "                Takes a performance measurement of the simulation, \n"
               "                implicitly sets the -n flag and deactivates logging "
               "entirely.\n"
               "                \n"
               "        -r, --hit-rate\n"
               "               Measures the hit-rate of the pairwise force "
               "calculation.\n"
               "               It's defined as the number of pairwise force "
               "calculations which \n"
               "               were within the cutoff radius (a hit), divided by the "
               "total \n"
               "               number of pairwise force calculations."
               "        \n"
               "        -v, --verbose\n"
               "                If specified the log-level is lowered from INFO to "
               "DEBUG.\n"
               "                If specified twice the log-level is even lowered to "
               "TRACE.\n"
               "                \n"
               "        -q, --quiet\n"
               "                Set loglevel to ERROR. Overrites -v.\n"
               "\n"
               "        -h, --help\n"
               "                Prints this help screen."
            << std::endl;
}
