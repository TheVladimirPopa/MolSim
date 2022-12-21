#pragma once
#include "ILineProcessor.h"

class LineProcessorCheckpoint : public ILineProcessor {
 public:
  void processLine(std::istringstream &s, IContainer &container) override;

 private:
  double getNext(std::istringstream &dataStream);

  template <std::size_t size>
  std::array<double, size> getNextArray(std::istringstream &dataStream);
};

