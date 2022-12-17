//
// Created by leo on 01.11.22.
//
#pragma once
#include <fstream>
#include <functional>
#include <vector>

#include "IContainer.h"
#include "Particle.h"

class VectorContainer : public IContainer {
 public:
  void forEach(std::function<void(Particle &)> &unaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      unaryFunction(vector[i]);
    }
  }

  void forEachPair(
      std::function<void(Particle &, Particle &)> &binaryFunction) override {
    for (size_t i = 0; i < size(); ++i) {
      for (size_t j = i + 1; j < size(); ++j) {
        binaryFunction(vector[i], vector[j]);
      }
    }
  }

  void checkpoint() override {
    std::ofstream checkpointFile;
    checkpointFile.open("checkpoint.txt");
    for (const Particle &p : vector) {
      checkpointFile << &p.getX() << "\t" << &p.getV() << "\t" << &p.getOldF()
                     << "\t" << p.getM() << "\t" << p.getType() << "\n";
    }
    checkpointFile.flush();
    checkpointFile.close();
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
