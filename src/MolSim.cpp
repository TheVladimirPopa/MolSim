
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
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

constexpr double start_time = 0;
constexpr double end_time = 10; // DEFAULT 1000
constexpr double delta_t = 0.014; // DEFAULT 0.014

// TODO: what data structure to pick?
std::list<Particle> particles;

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  FileReader fileReader;
  fileReader.readFile(particles, argsv[1]);

  // Strategy pattern, rough idea:
  ParticleContainer particleContainer;
  for (auto p : particles)
    particleContainer.addParticle(p);

  NewtonsLawModel model;
  particleContainer.setModel(&model);
  // ----------------

  double current_time = start_time;
  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    model.iterate(particleContainer, delta_t);

    iteration++;
    // DEFAULT 10
    if (iteration % 50 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}





void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

  //outputWriter::XYZWriter writer;
  //writer.plotParticles(particles, out_name, iteration);

  outputWriter::VTKWriter writer;
  writer.initializeOutput(particles.size());
  for (auto &p : particles) {
    writer.plotParticle(p);
  }

  //writer.writeFile(out_name, iteration);
}
