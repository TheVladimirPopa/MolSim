#include "CheckpointFileWriter.h"

#include <iostream>

void CheckpointFileWriter::writeFile(const std::string& filename, int iteration, IContainer& particles) {
  std::function<void(Particle&)> plot{[this](Particle& p) { this->checkpointParticle(p); }};
  writeFile(particles.size());
  particles.forEach(plot);
}

void CheckpointFileWriter::writeFile(size_t size) {
  file << "#\n"
          "# file format:\n"
          "# Lines of comment start with '#' and are only allowed at the beginning of the file\n"
          "# Empty lines are not allowed.\n"
          "# The first line not being a comment has to be one integer, indicating the number of\n"
          "# particles.\n"
          "#\n"
          "# Molecule data consists of\n"
          "# * xyz-coordinates (3 double values)\n"
          "# * current velocities (3 double values)\n"
          "# * old forces (3 double values)\n"
          "# * forces (3 double values)\n"
          "# * mass of one particle (1 double value)\n"
          "# * type of particles (1 integer)\n"
          "# * epsilon (1 double value)\n"
          "# * sigma (1 double value)\n"
          "# xyz-coord      velocity           oldF            force              mass        type      "
          "epsilon     sigma\n";

  file.precision(17);
  file << size << "\n";
}

void CheckpointFileWriter::checkpointParticle(const Particle& p) {
  if (file.is_open()) {
    file << p.getX().at(0) << " " << p.getX().at(1) << " " << p.getX().at(2) << "\t\t";
    file << p.getV().at(0) << " " << p.getV().at(1) << " " << p.getV().at(2) << "\t\t";
    file << p.getOldF().at(0) << " " << p.getOldF().at(1) << " " << p.getOldF().at(2) << "\t\t";
    file << p.getF().at(0) << " " << p.getF().at(1) << " " << p.getF().at(2) << "\t\t";
    file << p.getM() << "\t\t\t";
    file << p.getType() << "\t\t\t";
    file << p.getEpsilon() << "\t\t\t";
    file << p.getSigma() << "\n";
  }
}

CheckpointFileWriter::CheckpointFileWriter(const std::string& path) { file.open(path, std::ios::out); }

CheckpointFileWriter::~CheckpointFileWriter() {
  file.flush();
  file.close();
}
