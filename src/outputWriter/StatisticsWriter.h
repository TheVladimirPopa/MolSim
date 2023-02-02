#pragma once
#include <fstream>
#include <iostream>

#include "IWriter.h"
#include "utils/ThermodynamicStatistics.h"

class StatisticsWriter : public IWriter {
 private:
  int frequency;
  std::ofstream file;
  IContainer &particleContainer;
  ThermodynamicStatistics stat{};
  const std::string filename;
  double rdfDeltaR;
  double rdfStart;
  double rdfEnd;

 private:
  /**
   * Writes the statistics file header and number of particles into the file
   * @param size the number of particles
   */
  void writeFile(size_t size);

  /**
   * Writes the particle data into the file
   * @param iteration the current iteration
   */
  void registerParticle(int iteration);

 public:
  /**
   * Constructs a public StatistiWriter for the given container
   * @param particleContainer the given IContainer (either VectorContainer or LinkedCellsContainer
   */
  explicit StatisticsWriter(IContainer &particleContainer, int frequency, const std::string &filename, double rdfDeltaR,
                            double rdfStart, double rdfEnd);
  /**
   * Writes the statistics of each particle
   * @param filename
   * @param iteration
   * @param particles
   */
  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

  int getFrequency() const;

  const std::string &getFilename() const;

  virtual ~StatisticsWriter();
};