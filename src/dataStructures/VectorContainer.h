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
    for (auto& molecule : moleculesVector) molecule.applyInternalForces();

    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = i + 1; j < size(); ++j) {
        binaryFunction(vector[i], vector[j]);
      }
    }
  }

  size_t size() override { return vector.size(); }

  size_t capacity() override { return vector.capacity(); }

  void reserve(size_t amount) override { vector.reserve(amount); }

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type) override {
    vector.emplace_back(x_arg, v_arg, m_arg, type);
  };

  void push_back(Particle &particle) override { vector.push_back(std::forward<Particle &>(particle)); }

  void push_back(MembraneMolecule &&membrane) override {
    moleculesVector.push_back(membrane);
  }

  std::vector<Particle> &getVector() { return vector; };

 private:
  /// The vector containing all the particles
  std::vector<Particle> vector{};

  /// The vector containing all molecules. (Only membranes for now, worksheet 5)
  std::vector<MembraneMolecule> moleculesVector;
};
