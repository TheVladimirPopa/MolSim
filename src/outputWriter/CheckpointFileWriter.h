#pragma once
#include <fstream>

#include "VTKWriter.h"
class CheckpointFileWriter : public VTKWriter {
 private:
  std::ofstream file;

 public:
  CheckpointFileWriter();
  ~CheckpointFileWriter() override;
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

 private:
  void checkpointParticle(const Particle &p);
  void writeFile(size_t size);
};