
#include <getopt.h>

#include <functional>
#include <iostream>

#include "Simulation.h"
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "inputReader/FileReader.h"
#include "model/IModel.h"
#include "model/NewtonsLawModel.h"
#include "outputWriter/IWriter.h"
#include "outputWriter/VTKWriter.h"

void printHelp();
void printUsage();
enum simTypes { Single, Cuboid };
const std::map<std::string, simTypes> simTypeStrings{
    {"single", simTypes::Single}, {"cuboid", simTypes::Cuboid}};

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  Simulation simulation{};
  char *inputFile;
  // Argument parsing form commandline has to change in the future but for now
  // it's fine :)

  int opt;
  static struct option long_options[] = {
      {"output-file", required_argument, nullptr, 'o'},
      {"input-file", required_argument, nullptr, 'f'},
      {"interactive", no_argument, nullptr, 'i'},
      {"end-time", required_argument, nullptr, 'e'},
      {"delta-t", required_argument, nullptr, 'd'},
      {"write-frequency", required_argument, nullptr, 'w'},
      {"help", no_argument, nullptr, 'h'},
      {nullptr, 0, nullptr, 0}};
  while ((opt = getopt_long(argc, argsv, "o:f:ie:d:w:h", long_options,
                            nullptr)) != -1) {
    switch (opt) {
      case 'o': {
        simulation.setFilename(optarg);
        break;
      }
      case 'f': {
        inputFile = optarg;
        break;
      }
      case 'i':
        break;
      case 'e': {
        simulation.setEndTime(std::stod(optarg));
        break;
      }
      case 'd': {
        simulation.setDeltaT(std::stod(optarg));
        break;
      }
      case 'w': {
        simulation.setWriteOutFrequency(std::stoi(optarg));
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

  VTKWriter writer{};
  ParticleContainer particleContainer{};
  FileReader::readFile(particleContainer, inputFile);

  NewtonsLawModel model{};
  model.setDeltaT(simulation.getDeltaT());
  simulation.simulate(model, particleContainer, writer);

  std::cout << "Output written. Terminating..." << std::endl;
  return 0;
}
void printUsage() {
  std::cout << "Usage\n"
               "        ./Molsim (-i|-f <inputfile>) [-o <outputfile>] [-e "
               "<endtime>]\n"
               "                 [-d <deltaT>] [-w <iteration-count>] \n"
               "For more information run ./Molsim -h"
            << std::endl;
}

void printHelp() {
  std::cout
      << "Usage\n"
         "        ./Molsim (-i|-f <inputfile>) [-o <outputfile>] [-e "
         "<endtime>]\n"
         "                 [-d <deltaT>] [-w <iteration-count>] \n"
         "\n"
         "OPTIONS:\n"
         "        -o <filepath>, --output-name=<filepath>\n"
         "                Use the given <filepath> as the path for the "
         "outputfiles(iteration number and file-ending are added "
         "automatically)\n"
         "                If not specified \"MD_vtk\" is used\n"
         "\n"
         "        -f <filepath>, --input-file=<filepath>\n"
         "                Use the file at the <filepath> as an input.\n"
         "                The -f option is mutally exclusive with -i.\n"
         "\n"
         "        -i, --interactive\n"
         "                Allows to interactively set up the simulation "
         "instead of reading in a inputfile.\n"
         "                The -i option is mutally exclusive with -f.\n"
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
