//
// Created by leo on 01.11.22.
//
#pragma once
#include <functional>
#include <vector>

#include "IContainer.h"
#include "Particle.h"
#include "utils/ArrayUtils.h"

/**  \image html runtime-directsum.png width=900px
 *    \image latex runtime-directsum.png "Runtime" width = [goodwidth]cm
 */
class VectorContainer : public IContainer {
 public:
  void forEach(std::function<void(Particle &)> &unaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      unaryFunction(vector[i]);
    }
  }

  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = i + 1; j < size(); ++j) {
        binaryFunction(vector[i], vector[j]);
      }
    }

    for (auto &structure : structuresVector) {
      if (structure.hasArtificalForces()) structure.applyArtificialForces();
    }
  }

  size_t size() override { return vector.size(); }

  size_t capacity() override { return vector.capacity(); }

  void reserve(size_t amount) override { vector.reserve(amount); }

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type) override {
    vector.emplace_back(x_arg, v_arg, m_arg, type);
    vector.back().setId(vector.size() - 1);
  };

  void push_back(Particle &particle) override {
    vector.push_back(std::forward<Particle &>(particle));
    vector.back().setId(vector.size() - 1);
  }

  void push_back(MembraneStructure membrane) override {
    structuresVector.push_back(membrane);
    structuresVector.back().setStructureVectorIndex(structuresVector.size() - 1);
  }

  std::vector<Particle> &getVector() { return vector; };

  [[nodiscard]] std::vector<Particle> &getParticlesRef() override { return getVector(); }

  /**
   * @return True if the container has structures
   */
  [[nodiscard]] bool containsStructures() override { return !structuresVector.empty(); };

  /**
   * Returns structures that are attached to the container
   * @return attached structures
   */
  [[nodiscard]] std::vector<MembraneStructure> &getStructureVectorRef() override { return structuresVector; };

 private:
  /// The vector containing all the particles
  std::vector<Particle> vector{};

  /// The vector containing all structures. (Only membranes for now, worksheet 5)
  std::vector<MembraneStructure> structuresVector;
};
