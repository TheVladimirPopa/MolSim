
#include <iostream>
#include <functional>


#include "FileReader.h"
#include "IModel.h"
#include "NewtonsLawModel.h"
#include "Particle.h"
#include "ParticleContainer.h"
#include "outputWriter/VTKWriter.h"

/**** forward declaration of the calculation functions ****/
/**
 * Run the particle simulation with the chosen model.
 * (This implements the strategy pattern.)
 * @param The chosen model for the simulation in each iteration.
 */
void simulate(IModel const& model, ParticleContainer& particles);

/**
 * plot the particles to a xyz-file
 */
void plotParticles(ParticleContainer& pContainer, int iteration);

constexpr double start_time = 0;
constexpr double end_time = 10; // DEFAULT 1000
constexpr double delta_t = 0.014; // DEFAULT 0.014


int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  FileReader fileReader;
  std::vector<Particle> particles;
  fileReader.readFile(particles, argsv[1]);

  ParticleContainer particleContainer{particles};
  NewtonsLawModel model{};
  model.setDeltaT(delta_t);
  simulate(model, particleContainer);

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}


void simulate(IModel const& model, ParticleContainer& pContainer) {
  double current_time = start_time;
  int iteration = 0;

  // This is wrapper code that hopefully we can replace in the future
  std::function<void(Particle &)> updateX = [&model](Particle & p){
    model.updateX(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateV = [&model](Particle &p){
    model.updateV(std::forward<Particle &>(p));
  };

  std::function<void(Particle &)> updateF = [](Particle &p){
    p.updateForces();
  };

  std::function<void(Particle &, Particle &)> addForces =
    [&model](Particle &p1, Particle &p2){
      model.addForces(std::forward<Particle &>(p1), std::forward<Particle &>(p2));
    };

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    pContainer.forEach(updateX);
    pContainer.forEach(updateV);
    pContainer.forEach(updateF);
    pContainer.forEachPair(addForces);

    // DEFAULT 10
    if (iteration % 50 == 0) {
      plotParticles(pContainer, iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
    iteration++;
  }
}




void plotParticles(ParticleContainer& pContainer, int iteration) {
  std::string out_name("MD_vtk");

  outputWriter::VTKWriter writer;
  writer.initializeOutput(pContainer.getVectorRef().size());

  std::function<void(Particle &)> plotFun = [&writer](Particle &p){
    writer.plotParticle(std::forward<Particle &>(p));
  };
  pContainer.forEach(plotFun);

  writer.writeFile(out_name, iteration);
}
