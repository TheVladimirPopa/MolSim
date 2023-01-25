#pragma once
#include <fstream>
#include <iostream>

#include "IWriter.h"

class StatisticsWriter : public IWriter {
 private:
  std::ofstream file;
  IContainer *particleContainer;

  /**
   * Writes the statistics file header and number of particles into the file
   * @param size the number of particles
   */
  void writeFile(size_t size);

  /**
   * Writes the particle data into the file
   * @param p the given particle
   */
  void registerParticle(const Particle &p);

 public:
  /**
   * Constructs a public StatistiWriter for the given container
   * @param particleContainer the given IContainer (either VectorContainer or LinkedCellsContainer
   */
  explicit StatisticsWriter(IContainer *particleContainer);
  /**
   * Writes the statistics of each particle
   * @param filename
   * @param iteration
   * @param particles
   */
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

  virtual ~StatisticsWriter();
};