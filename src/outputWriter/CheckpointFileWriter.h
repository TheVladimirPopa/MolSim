#pragma once
#include <fstream>

#include "VTKWriter.h"
class CheckpointFileWriter : public IWriter {
 private:
  std::ofstream file;

 public:
  explicit CheckpointFileWriter(const std::string &path);
  ~CheckpointFileWriter();
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

 private:
  void checkpointParticle(const Particle &p);
  void writeFile(size_t size);
};