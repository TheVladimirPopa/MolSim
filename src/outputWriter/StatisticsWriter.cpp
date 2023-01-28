#include "StatisticsWriter.h"

#include <iostream>

#include "utils/ThermodynamicStatistics.h"
#define START 1
#define END 50
#define DELTA_R 1

void StatisticsWriter::writeFile(const std::string& filename, int iteration, IContainer& particles) {
  file.open(filename, std::ios::out | std::ios::app);
  if (iteration == frequency) {
    file.precision(20);
    writeFile(particles.size());
  }
  registerParticle(iteration);
  file.flush();
  file.close();
}

void StatisticsWriter::writeFile(size_t size) {
  file << "#\n"
          "# file format:\n"
          "# Lines of comment start with '#' and are only allowed at the beginning of the file\n"
          "# Empty lines are not allowed.\n"
          "# The first line not being a comment has to be one integer, indicating the number of\n"
          "# particles.\n"
          "#\n"
          "# Molecule data consists of\n"
          "# * iteration (integer value)\n"
          "# * var (double value)\n"
          "# * radialDistributionFunction (vector of double values)\n";
  file << size << " Particles"
       << "\n";
}

void StatisticsWriter::registerParticle(int iteration) {
  if (file.is_open()) {
    file << "Iteration: " << iteration << "\n"
         << "Var(t): ";
    // rdf: delta_r is set to 1 and i is in [1, 50]
    if (instanceof <LinkedCellsContainer>(particleContainer)) {
      file << ThermodynamicStatistics::var(reinterpret_cast<LinkedCellsContainer&>(*particleContainer),
                                           iteration / frequency, iteration / frequency - 1)
           << "\n"
           << "RDF: ";
      file << ThermodynamicStatistics::radialDistributionFunction(
                  DELTA_R, reinterpret_cast<LinkedCellsContainer&>(*particleContainer), START, END)
           << "\n";
    } else {
      file << ThermodynamicStatistics::var(reinterpret_cast<VectorContainer&>(*particleContainer),
                                           iteration / frequency, iteration / frequency - 1)
           << "\n"
           << "RDF: ";
      file << ThermodynamicStatistics::radialDistributionFunction(
                  DELTA_R, reinterpret_cast<VectorContainer&>(*particleContainer), START, END)
           << "\n";
    }
    file << "\n";
  }
}

StatisticsWriter::StatisticsWriter(IContainer* particleContainer, int frequency)
    : frequency{frequency}, particleContainer{particleContainer} {}

StatisticsWriter::~StatisticsWriter() = default;
