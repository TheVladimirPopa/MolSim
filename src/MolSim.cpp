
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
void plotParticles(ParticleContainer& pContainer);

constexpr double start_time = 0;
constexpr double end_time = 100; // DEFAULT 1000
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

// todo, model should be const
void simulate(IModel const& model, ParticleContainer& pContainer) {
  double current_time = start_time;
  int iteration = 0;


  // todo: There has to be a better way for this kind of wrapping (js):
  std::function<void(Particle &)> xFun = [&model](Particle &p){ model.updateX(p); };
  std::function<void(Particle &)> vFun = [&model](Particle &p){ model.updateV(p); };
  std::function<void(Particle &)> resetFun = [](Particle &p){ p.resetForces(); };
  std::function<void(Particle &, Particle &)> forcesFun = [&model](Particle &p1, Particle &p2){ model.addForces(p1, p2); };


  //std::function<void(Particle &)> debug = [](Particle &p) { std::cout << p.toString() << '\n'; }; // todo remove this

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    pContainer.forEach(xFun);
    pContainer.forEach(vFun);
    pContainer.forEach(resetFun);
    pContainer.forEachPair(forcesFun);
    //pContainer.forEach(debug); // todo remove this

    // DEFAULT 10
    if (iteration % 50 == 0) {
      plotParticles(pContainer);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
    iteration++;
  }
}




void plotParticles(ParticleContainer& pContainer) {
  std::string out_name("MD_vtk");

  outputWriter::VTKWriter writer;
  writer.initializeOutput(particles.size());

  std::function<void(Particle &)> plotFun = [&writer](Particle &p){ writer.plotParticle(p); };
  pContainer.forEach(plotFun);


  // todo: uncomment code
  // writer.writeFile(out_name, iteration);
}
