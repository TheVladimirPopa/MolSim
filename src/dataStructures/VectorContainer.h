//
// Created by leo on 01.11.22.
//
#pragma once
#include <functional>
#include <vector>

#include "IContainer.h"
#include "Particle.h"

class VectorContainer : public IContainer {
 public:
  /**
   * Applies a function to all particles in the container
   * @param unaryFunction A function taking a particle reference and returning
   * nothing
   */
  void forEach(std::function<void(Particle &)> &unaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      unaryFunction(vector[i]);
    }
  }

  /**
   * Applies a function to all particle pairs in the container
   * @param binaryFunction A function taking two particle references and
   * returning nothing
   * @note A pair is considered without order, so if the function got applied to
   * (p1,p2), it won't get applied to (p2,p1). Also pairs with the same particle
   * are not considered
   */
  void forEachPair(
      std::function<void(Particle &, Particle &)> &binaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = i + 1; j < size(); ++j) {
        binaryFunction(vector[i], vector[j]);
      }
    }
  }

  size_t size() override { return vector.size(); }

  size_t capacity() override { return vector.capacity(); }

  void reserve(size_t amount) override { vector.reserve(amount); }

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                    double m_arg, int type) override {
    vector.emplace_back(x_arg, v_arg, m_arg, type);
  };

  std::vector<Particle> &getVector() { return vector; };

 private:
  std::vector<Particle> vector{};
};
