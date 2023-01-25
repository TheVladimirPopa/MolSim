#include "StatisticsWriter.h"

#include <iostream>

#include "utils/ThermodynamicStatistics.h"

void StatisticsWriter::writeFile(const std::string& filename, int iteration, IContainer& particles) {
  file.open(filename, std::ios::out);
  std::function<void(Particle&)> plot{[this](Particle& p) { this->registerParticle(p); }};
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
          "# * xyz-coordinates (3 double values)\n"
          "# * current time (double value)\n"
          "# * reference time (double value)\n"
          "# * var (double value)\n"
          "# * radialDistributionFunction (double value)\n"
          "# xyz-coord      t           t_0            var           rdf";

  file.precision(17);
  file << size << "\n";
}

void StatisticsWriter::registerParticle(const Particle& p) {
  if (file.is_open()) {
    file << ThermodynamicStatistics::var(particleContainer, 0, 0);
    file << ThermodynamicStatistics::radialDistributionFunction(1, particleContainer, 1, 50);
  }
}

StatisticsWriter::StatisticsWriter(IContainer* particleContainer) : particleContainer{particleContainer} {}

StatisticsWriter::~StatisticsWriter() = default;
