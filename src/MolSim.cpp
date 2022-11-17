
#include <getopt.h>

#include <chrono>
#include <cmath>
#include <iostream>

#include "Simulation.h"
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "inputReader/FileReader.h"
#include "model/LennardJonesModel.h"
#include "outputWriter/NoWriter.h"
#include "outputWriter/VTKWriter.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

void printHelp();
void printUsage();
enum simTypes { Single, Cuboid };
const std::map<std::string, simTypes> simTypeStrings{
    {"single", simTypes::Single}, {"cuboid", simTypes::Cuboid}};

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;

  // Print help when no arguments or options are present
  if (argc == 1) {
    printUsage();
    return 1;
  }

  Simulation simulation{};
  char *inputFile = nullptr;
  bool noOutput = false;
  simTypes simulationType = simTypes::Single;
  int loglevel = 0;
  bool quietLog = false;
  bool performanceMeasure = false;

  int opt;
  static struct option long_options[] = {
      {"output-file", required_argument, nullptr, 'o'},
      {"no-output", no_argument, nullptr, 'n'},
      {"type", required_argument, nullptr, 't'},
      {"input-file", required_argument, nullptr, 'f'},
      {"end-time", required_argument, nullptr, 'e'},
      {"delta-t", required_argument, nullptr, 'd'},
      {"write-frequency", required_argument, nullptr, 'w'},
      {"performance", no_argument, nullptr, 'p'},
      {"verbose", no_argument, nullptr, 'v'},
      {"quiet", no_argument, nullptr, 'q'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, 0, nullptr, 0}};

  while ((opt = getopt_long(argc, argsv, "o:nt:f:e:d:w:pvqh", long_options,
                            nullptr)) != -1) {
    switch (opt) {
      case 'o': {
        simulation.setFilename(optarg);
        break;
      }
      case 'n': {
        noOutput = true;
        break;
      }
      case 't': {
        auto pos = simTypeStrings.find(optarg);
        if (pos == simTypeStrings.end()) {
          std::cout << "Type " << optarg << " is not known" << std::endl;
          printUsage();
        } else {
          simulationType = pos->second;
        }
        break;
      }
      case 'f': {
        inputFile = optarg;
        break;
      }
      case 'e': {
        try {
          double end = std::stod(optarg);
          if (end <= 0) {
            std::cout << "End-time has to be positive but is " << end
                      << std::endl;
            printUsage();
            return 1;
          } else {
            simulation.setEndTime(end);
          }
        } catch (std::invalid_argument &e) {
          std::cout << "End-time has to be double, but failed to convert "
                    << optarg << std::endl;
          printUsage();
          return 1;
        }

        break;
      }
      case 'd': {
        simulation.setDeltaT(std::stod(optarg));
        try {
          double delta = std::stod(optarg);
          if (delta <= 0) {
            std::cout << "DeltaT has to be positive but is " << delta
                      << std::endl;
            printUsage();
            return 1;
          } else {
            simulation.setDeltaT(delta);
          }
        } catch (std::invalid_argument &e) {
          std::cout << "DeltaT has to be double, but failed to convert "
                    << optarg << std::endl;
          printUsage();
          return 1;
        }

        break;
      }
      case 'w': {
        try {
          int frequency = std::stoi(optarg);
          if (frequency <= 0) {
            std::cout << "WriteOutFrequency has to be positive integer but "
                      << frequency << " is negative" << std::endl;
            printUsage();
            return 1;
          } else {
            simulation.setIterationsPerWrite(frequency);
          }
        } catch (std::invalid_argument &e) {
          std::cout << "WriteOutFrequency has to be positive integer but "
                       "failed to convert "
                    << optarg << std::endl;
          printUsage();
          return 1;
        }

        break;
      }
      case 'p': {
        performanceMeasure = true;
        break;
      }
      case 'v': {
        loglevel += 1;
        break;
      }
      case 'q': {
        quietLog = true;
        break;
      }
      case 'h': {
        printHelp();
        return 0;
      }
      case '?':

      default: {
        printUsage();
        return 1;
      }
    }
  }

  if (inputFile == nullptr) {
    std::cout << "You have to specify an input file with -f flag" << std::endl;
    printUsage();
    return 1;
  }
  auto file_logger =
      spdlog::rotating_logger_st("file_logger", "logs/logger", 1048576 * 5, 5);

  auto console_logger = spdlog::stdout_color_st("console_logger");

  if (performanceMeasure) {
    noOutput = true;
  }
  if (quietLog) {
    console_logger->set_level(spdlog::level::err);
    file_logger->set_level(spdlog::level::info);
  } else {
    switch (loglevel) {
      case 0:
        console_logger->set_level(spdlog::level::info);
        file_logger->set_level(spdlog::level::info);
        break;
      case 1:
        console_logger->set_level(spdlog::level::debug);
        file_logger->set_level(spdlog::level::debug);
        break;
      default:
        console_logger->set_level(spdlog::level::trace);
        file_logger->set_level(spdlog::level::trace);
        break;
    }
  }

  ParticleContainer particleContainer{};
  switch (simulationType) {
    case simTypes::Single: {
      FileReader::readFileSingle(particleContainer, inputFile);
      break;
    }
    case simTypes::Cuboid: {
      FileReader::readFileCuboid(particleContainer, inputFile);
      break;
    }
  }

  LennardJonesModel model{};
  model.setDeltaT(simulation.getDeltaT());

  VTKWriter vtkWriter{};
  NoWriter noWriter{};

  IWriter *selectedWriter = &noWriter;
  if (noOutput) {
    spdlog::info("No output will b written.");
  } else {
    selectedWriter = &vtkWriter;
  }

  if (performanceMeasure) {
    spdlog::info(
        "Setup done! Deactivating logging and starting performance measurement "
        "now...");

    console_logger->set_level(spdlog::level::off);
    file_logger->set_level(spdlog::level::off);
  }

  auto startTime = std::chrono::steady_clock::now();

  simulation.simulate(model, particleContainer, *selectedWriter);

  if (performanceMeasure) {
    auto endTime = std::chrono::steady_clock::now();
    auto durationSec =
        std::chrono::duration<double>{endTime - startTime}.count();
    auto iterationCount =
        std::ceil(simulation.getEndTime() / simulation.getDeltaT());
    std::cout << "Results of performance measurement\n"
                 "Execution time       : "
              << durationSec
              << "s\n"
                 "Number of iterations : "
              << static_cast<unsigned long>(iterationCount)
              << "\n"
                 "Time per iteration   : "
              << (durationSec / iterationCount) << "s" << std::endl;
  }

  spdlog::info("Terminating...");
  return 0;
}

void printUsage() {
  std::cout
      << " Usage\n"
         "        ./MolSim -f <input-file> [-t (single|cuboid)] [-o "
         "<output-file>] [-e <endtime>]\n"
         "                                [-d <deltaT>] [-w <iteration-count>] "
         "[-n] [-p] [-v] [-v] [-q]\n"
         "\n"
         "For more information run ./Molsim -h or ./Molsim --help"
      << std::endl;
}

void printHelp() {
  std::cout
      << "Usage\n"
         "        ./MolSim -f <input-file> [-t (single|cuboid)] [-o "
         "<output-file>] [-e <endtime>]\n"
         "                                [-d <deltaT>] [-w <iteration-count>] "
         "[-n] [-p] [-v] [-v] [-q]\n"
         "\n"
         "OPTIONS:\n"
         "        -o <filepath>, --output-name=<filepath>\n"
         "                Use the given <filepath> as the path for the "
         "outputfiles(iteration number and file-ending are added "
         "automatically)\n"
         "                If not specified \"MD_vtk\" is used\n"
         "                \n"
         "        -n, --no-output\n"
         "                If active no files will be written, even overwrites "
         "-o.\n"
         "\n"
         "        -t (single|cuboid), --type=(single|cuboid)\n"
         "                Specifies the way to set up particles (default is "
         "single).\n"
         "                Use single if you want particles on their own and "
         "use cuboid if you want the particles to spawn in cuboids.\n"
         "\n"
         "        -f <filepath>, --input-file=<filepath>\n"
         "                Use the file at the <filepath> as an input.\n"
         "\n"
         "        -e <endtime>, --end-time=<endtime>\n"
         "                The time until the simulation is run (default is "
         "1000).\n"
         "\n"
         "        -d <deltaT>, --delta-t=<deltaT>\n"
         "                The timestep by which the time gets increased in "
         "every iteration (default is 0.014).\n"
         "\n"
         "        -w <iteration-count>, --write-frequency=<iteration-count>\n"
         "                Every <iteration-count>nth iteration the particles "
         "get written to a file (default is 10).\n"
         "        \n"
         "        -p, --performance\n"
         "                Takes a performace measurement of the simulation, "
         "implicitly sets the -n flag and deactivates logging entirely.\n"
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
