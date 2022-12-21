//
// Created by leo on 01.11.22.
//
#pragma once
#include <fstream>
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
  }

  void checkpoint() override {
    std::fstream checkpointFile;
    checkpointFile.open("../input/checkpoint.txt", std::ios::out);
    checkpointFile << "#\n"
                      "# file format:\n"
                      "# Lines of comment start with '#' and are only allowed at the beginning of the file\n"
                      "# Empty lines are not allowed.\n"
                      "# The first line not being a comment has to be one integer, indicating the number of\n"
                      "# particles.\n"
                      "#\n"
                      "# Molecule data consists of\n"
                      "# * xyz-coordinates (3 double values)\n"
                      "# * current velocities (3 double values)\n"
                      "# * old forces (3 double values)\n"
                      "# * forces (3 double values)\n"
                      "# * mass of one particle (1 double value)\n"
                      "# * type of particles (1 integer)\n"
                      "# * epsilon (1 double value)\n"
                      "# * sigma (1 double value)\n"
                      "# xyz-coord      velocity           oldF            force              mass        type      "
                      "epsilon     sigma\n";
    checkpointFile << vector.size() << "\n";
    for (const Particle &p : vector) {
      if (checkpointFile.is_open()) {
        checkpointFile << p.getX().at(0) << " " << p.getX().at(1) << " " << p.getX().at(2) << "\t\t";
        checkpointFile << p.getV().at(0) << " " << p.getV().at(1) << " " << p.getV().at(2) << "\t\t";
        checkpointFile << p.getOldF().at(0) << " " << p.getOldF().at(1) << " " << p.getOldF().at(2) << "\t\t";
        checkpointFile << p.getF().at(0) << " " << p.getF().at(1) << " " << p.getF().at(2) << "\t\t";
        checkpointFile << p.getM() << "\t\t\t";
        checkpointFile << p.getType() << "\t\t\t";
        checkpointFile << p.getEpsilon() << "\t\t\t";
        checkpointFile << p.getSigma() << "\n";
      }
    }
    checkpointFile.flush();
    checkpointFile.close();
  }

  size_t size() override { return vector.size(); }

  size_t capacity() override { return vector.capacity(); }

  void reserve(size_t amount) override { vector.reserve(amount); }

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type) override {
    vector.emplace_back(x_arg, v_arg, m_arg, type);
  };

  void push_back(Particle &particle) override { vector.push_back(std::forward<Particle &>(particle)); }

  std::vector<Particle> &getVector() { return vector; };

 private:
  std::vector<Particle> vector{};
};
