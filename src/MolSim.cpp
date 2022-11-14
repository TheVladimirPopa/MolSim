
#include <getopt.h>

#include <iostream>

#include "Simulation.h"
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "inputReader/FileReader.h"
#include "model/NewtonsLawModel.h"
#include "model/LennardJonesModel.h"
#include "outputWriter/NoWriter.h"
#include "outputWriter/VTKWriter.h"

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

  int opt;
  static struct option long_options[] = {
      {"output-file", required_argument, nullptr, 'o'},
      {"no-output", no_argument, nullptr, 'n'},
      {"type", required_argument, nullptr, 't'},
      {"input-file", required_argument, nullptr, 'f'},
      {"end-time", required_argument, nullptr, 'e'},
      {"delta-t", required_argument, nullptr, 'd'},
      {"write-frequency", required_argument, nullptr, 'w'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, 0, nullptr, 0}};

  while ((opt = getopt_long(argc, argsv, "o:nt:f:e:d:w:h", long_options,
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
  if (!noOutput) selectedWriter = &vtkWriter;

  simulation.simulate(model, particleContainer, *selectedWriter);

  std::cout << "Output written. Terminating..." << std::endl;
  return 0;
}
void printUsage() {
  std::cout << "Usage\n"
               "        ./Molsim -f <input-file> [-t (single|cuboid)] [-o "
               "<output-file>]\n"
               "                [-e <endtime>] [-d <deltaT>] [-w "
               "<iteration-count>] [-n]\n"
               "\n"
               "For more information run ./Molsim -h or ./Molsim --help"
            << std::endl;
}

void printHelp() {
  std::cout
      << "        ./Molsim -f <input-file> [-t (single|cuboid)] [-o "
         "<output-file>]\n"
         "                [-e <endtime>] [-d <deltaT>] [-w <iteration-count>] "
         "[-n]\n"
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
         "\n"
         "        -h, --help\n"
         "                Prints this help screen."
      << std::endl;
}
