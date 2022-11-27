//
// Created by leo on 12.11.22.
//
#include "IWriter.h"
class NoWriter : public IWriter {
  void writeFile(const std::string &filename, int iteration,
                 IContainer &particles) override {}
};
