#pragma once
#include <fstream>
#include <iostream>

#include "IWriter.h"

class StatisticsWriter : public IWriter {
 private:
  std::ofstream file;

 public:
  explicit StatisticsWriter();

  void writeFile(const std::string &filename, int iteration, IContainer &particles) override;

  void writeFile();

};