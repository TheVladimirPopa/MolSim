
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

/**
 * Usage: ./MolSim inputfile end_time delta_t
 * @param argc Has to be 4 in order to run
 * @param argsv argsv[1] = inputfilePath;
 *  argsv[2]= end_time;
 *  argsv[3]= delta_t (stepwidth)
 * @return 0 iff the simulation and I/O was successful
 */
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
      {"--help", no_argument, nullptr, 'h'},
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
      case '?':
      case 'h':
      default: {
        std::cout << "HELP" << std::endl;
        return 0;
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


