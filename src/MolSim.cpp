
#include <functional>
#include <iostream>

#include "FileReader.h"
#include "IModel.h"
#include "NewtonsLawModel.h"
#include "Particle.h"
#include "ParticleContainer.h"
#include "outputWriter/OutputModel.h"
#include "outputWriter/VTKWriter.h"

/**** forward declaration of the calculation functions ****/
/**
 * Run the particle simulation with the chosen model.
 * (This implements the strategy pattern.)
 * @param The chosen model for the simulation in each iteration.
 */
void simulate(IModel const &model, ParticleContainer &particles,
              OutputModel &outputModel);

constexpr double start_time = 0;
constexpr double end_time = 10;    // DEFAULT 1000
constexpr double delta_t = 0.014;  // DEFAULT 0.014

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  VTKWriter writer = VTKWriter{};
  ParticleContainer particleContainer{};
  FileReader::readFile(particleContainer, argsv[1]);

  NewtonsLawModel model{};
  model.setDeltaT(delta_t);
  simulate(model, particleContainer, writer);

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}

void simulate(IModel const &model, ParticleContainer &particles,
              OutputModel &outputModel) {
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
    particles.forEach(updateV);
    particles.forEach(updateF);
    particles.forEachPair(addForces);

    // DEFAULT 10
    if (iteration % 50 == 0) {
      outputModel.writeFile("MD_vtk", iteration, particles);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
    iteration++;
  }
}
