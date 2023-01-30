#pragma once

#include <iostream>

class StatArg {
 private:
  int frequency;
  double rdfDeltaR;
  double rdfStart;
  double rdfEnd;
  std::string path;

 public:
  StatArg(int frequency, double rdfDeltaR, double rdfStart, double rdfEnd, std::string  path);

  [[nodiscard]] int getFrequency() const;

  [[nodiscard]] double getRdfDeltaR() const;

  [[nodiscard]] double getRdfStart() const;

  [[nodiscard]] double getRdfEnd() const;

  [[nodiscard]] const std::string& getPath() const;
};