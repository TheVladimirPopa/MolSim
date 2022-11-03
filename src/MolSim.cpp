
#include <functional>
#include <iostream>

#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "inputReader/FileReader.h"
#include "model/IModel.h"
#include "model/NewtonsLawModel.h"
#include "outputWriter/IWriter.h"
#include "outputWriter/VTKWriter.h"

/**** forward declaration of the calculation functions ****/
/**
 * Simulate the particles with the given model and stepwidth of delta_t
 * The start_time and end_time variable are used to determine the duration of
 * the simulation
 * @param model Defining the model to update x, f and v in each iteration
 * @param particles All the particles that take part in the simulation
 * @param fileWriter A way of outputting the results every nth iterations to a
 * file
 */
void simulate(IModel const &model, ParticleContainer &particles,
              IWriter &fileWriter);

constexpr double start_time = 0;
double end_time = 1000;  // DEFAULT 1000
double delta_t = 0.014;  // DEFAULT 0.014

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

  char *err_ptr1;
  char *err_ptr2;

  if (argc == 4) {
    end_time = strtod(argsv[2], &err_ptr1);
    delta_t = strtod(argsv[3], &err_ptr2);
  }

  if (argc != 4 || *err_ptr2 != '\0' || *err_ptr1 != '\0') {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename end_time delta_t" << std::endl;
    return 1;
  }

  VTKWriter writer{};
  ParticleContainer particleContainer{};
  FileReader::readFile(particleContainer, argsv[1]);

  NewtonsLawModel model{};
  model.setDeltaT(delta_t);
  simulate(model, particleContainer, writer);

  std::cout << "Output written. Terminating..." << std::endl;
  return 0;
}

void simulate(IModel const &model, ParticleContainer &particles,
              IWriter &fileWriter) {
  double current_time = start_time;
  int iteration = 0;

  // This is wrapper code that hopefully we can replace in the future
  std::function<void(Particle &)> updateX = [&model](Particle &p) {
    model.updateX(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateV = [&model](Particle &p) {
    model.updateV(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateF = [](Particle &p) {
    p.updateForces();
  };

  std::function<void(Particle &, Particle &)> addForces = [&model](
                                                              Particle &p1,
                                                              Particle &p2) {
    model.addForces(std::forward<Particle &>(p1), std::forward<Particle &>(p2));
  };

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    particles.forEach(updateX);

    particles.forEach(updateF);
    particles.forEachPair(addForces);

    particles.forEach(updateV);

    // DEFAULT 10
    if (iteration % 50 == 0) {
      fileWriter.writeFile("MD_vtk", iteration, particles);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
    iteration++;
  }
}
