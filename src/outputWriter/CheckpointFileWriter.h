#pragma once
#include <fstream>

#include "VTKWriter.h"
class CheckpointFileWriter : public IWriter {
 private:
  std::ofstream file;

 public:
  /**
   * Constructs a CheckpointFileWriter from the given path
   * @param path
   */
  explicit CheckpointFileWriter(const std::string &path);
  /**
   * Writes the checkpoint of each particle into the path file
   * @param filename unused
   * @param iteration unused
   * @param particles
   */
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

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