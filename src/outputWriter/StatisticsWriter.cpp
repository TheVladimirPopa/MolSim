#include "StatisticsWriter.h"

#include <iostream>

#include "utils/ThermodynamicStatistics.h"

void StatisticsWriter::writeFile(const std::string& filename, int iteration, IContainer& particles) {
  file.open(filename, std::ios::out);
  std::function<void(Particle&)> plot{[this, iteration](Particle& p) { this->registerParticle(p, iteration); }};
  writeFile(particles.size());
  particles.forEach(plot);
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
          "# * xyz-coordinates (3 double values)\n"
          "# * var (double value)\n"
          "# * radialDistributionFunction (vector of double values)\n"
          "# iteration      xyz-coord         var           rdf";

  file.precision(17);
  file << size << "\n";
}

void StatisticsWriter::registerParticle(const Particle& p, int iteration) {
  if (file.is_open()) {
    file << iteration << "\t\t";
    file << p.getX().at(0) << " " << p.getX().at(1) << " " << p.getX().at(2) << "\t\t";

    // rdf: delta_r is set to 1 and i is in [1, 50]
    if (instanceof <LinkedCellsContainer>(particleContainer)) {
      file << ThermodynamicStatistics::var(reinterpret_cast<LinkedCellsContainer&>(*particleContainer), iteration,
                                             iteration - 1)
           << "\t\t";
      file << ThermodynamicStatistics::radialDistributionFunction(
                  1, reinterpret_cast<LinkedCellsContainer&>(*particleContainer), 1, 50)
           << "\n";
    } else {
      file << ThermodynamicStatistics::var(reinterpret_cast<VectorContainer&>(*particleContainer), iteration,
                                             iteration - 1)
           << "\t\t";
      file << ThermodynamicStatistics::radialDistributionFunction(
                  1, reinterpret_cast<VectorContainer&>(*particleContainer), 1, 50)
           << "\n";
    }
  }
}

StatisticsWriter::StatisticsWriter(IContainer* particleContainer) : particleContainer{particleContainer} {}

StatisticsWriter::~StatisticsWriter() = default;
