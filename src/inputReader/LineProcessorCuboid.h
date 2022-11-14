//
// Created by leo on 12.11.22.
//
#pragma once
#include "ILineProcessor.h"
#include "ParticleGenerator.h"

class LineProcessorCuboid : public ILineProcessor {
 private:
  ParticleGeneration::cuboid cuboid{};

 public:
  void processLine(std::istringstream &dataStream,
                   ParticleContainer &container) override;
};
