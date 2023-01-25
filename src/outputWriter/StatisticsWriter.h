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
   * @param iteration the current iteration
   */
  void registerParticle(const Particle& p, int iteration);

  /**
   * Used to determine the class of an object (similar to instanceof in Java)
   * @tparam Base
   * @tparam T
   * @param ptr
   * @return
   */
  template <typename Base, typename T>
  inline bool instanceof (const T *ptr) {
    return dynamic_cast<const Base *>(ptr) != nullptr;
  }

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