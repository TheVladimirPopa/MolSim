
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
  // Argument parsing form commandline has to change in the future but for now
  // it's fine :)
  if (argc == 4) {
    try {
      simulation.setEndTime(std::stod(argsv[2]));
      simulation.setDeltaT(std::stod(argsv[3]));
    } catch (std::invalid_argument &e) {
      std::cout << "Couldn't convert to double" << std::endl;
      std::cout << "Usage: ./MolSim filename end_time delta_t" << std::endl;
      return 1;
    }
  } else {
    std::cout << "Wrong argument count" << std::endl;
    std::cout << "Usage: ./MolSim filename end_time delta_t" << std::endl;
    return 1;
  }

  std::cout << "Running simulation from " << argsv[1] << " until " << end_time
            << " with stepwidth of " << delta_t << std::endl;

  VTKWriter writer{};
  ParticleContainer particleContainer{};
  FileReader::readFile(particleContainer, argsv[1]);

  NewtonsLawModel model{};
  model.setDeltaT(delta_t);
  simulation.simulate(model, particleContainer, writer);

  std::cout << "Output written. Terminating..." << std::endl;
  return 0;
}


