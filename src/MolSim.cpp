
#include <iostream>
#include <list>

#include "FileReader.h"
#include "Particle.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

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



  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();

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

void calculateF() {
  std::list<Particle>::iterator iterator;
  iterator = particles.begin();

  for (auto &p1 : particles) {
      p1.resetForceIteration();
        for (auto &p2 : particles) {
            // @TODO: insert calculation of forces here! DONE
            if (!(p1 == p2)) p1.addForce(p2);
        }
    }
}

void calculateX() {
  for (auto &p : particles) {
    // @TODO: insert calculation of position updates here! DONE
        p.updateLocation(delta_t);
        std::cout << p.toString() << '\n';
  }
}

void calculateV() {
  for (auto &p : particles) {
    // @TODO: insert calculation of veclocity updates here! DONE
        p.updateVelocity(delta_t);
  }
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
