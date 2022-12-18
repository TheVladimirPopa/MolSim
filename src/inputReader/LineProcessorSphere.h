#pragma once
#include "ILineProcessor.h"
#include "ParticleGenerator.h"

class LineProcessorSphere : public ILineProcessor {
 private:
  ParticleGeneration::sphere sphere{};

 public:
  void processLine(std::istringstream &dataStream, IContainer &container) override;
};