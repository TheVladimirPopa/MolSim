#pragma once
#include <fstream>

#include "VTKWriter.h"
class CheckpointFileWriter : public IWriter {
 private:
  std::ofstream file;
  std::string outputFile;

 public:
  /**
   * Constructs a CheckpointFileWriter
   */
  CheckpointFileWriter(std::string outputFile);
  /**
   * Writes the checkpoint of each particle into the path file
   * @param filename unused
   * @param iteration unused
   * @param particles
   */
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

  const std::string &getOutputFile() const;

  ~CheckpointFileWriter();

 private:
  /**
   * Writes the data of each particle into the path file
   * @param p
   */
  void checkpointParticle(const Particle &p);
  /**
   * Writes the checkpoint file header and the number of particles into the path file
   * @param size
   */
  void writeFile(size_t size);
};