#pragma once

#include <iostream>

class ParticleArg {
 private:
  int id;
  double epsilon;
  double sigma;

 public:
  ParticleArg(int id, double epsilon, double sigma);

  [[nodiscard]] int getId() const;

  [[nodiscard]] double getEpsilon() const;

  [[nodiscard]] double getSigma() const;
};