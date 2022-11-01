
#include <iostream>
#include <list>
#include "utils/Interfaces.h"
#include "FileReader.h"
#include "NewtonsLawModel.h"
#include "Particle.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"


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
void plotParticles(int iteration);

constexpr double start_time = 0;
constexpr double end_time = 10; // DEFAULT 1000
constexpr double delta_t = 0.014; // DEFAULT 0.014

// TODO: what data structure to pick?
std::vector<Particle> particles;

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  FileReader fileReader;
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

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    pContainer.forEach(model.updateX);
    pContainer.forEach(model.updateV);
    pContainer.forEach(model.resetForce);
    pContainer.forEachPair(model.updateForce);

    // DEFAULT 10
    if (iteration % 50 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }
}




void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

  outputWriter::VTKWriter writer;
  writer.initializeOutput(particles.size());
  for (auto &p : particles) {
    writer.plotParticle(p);
  }

  // todo: output code
  //writer.writeFile(out_name, iteration);
}
