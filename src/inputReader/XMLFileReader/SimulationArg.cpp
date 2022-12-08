#include "SimulationArg.h"

#include <utility>
SimulationArg::SimulationArg(double endTime, double deltaT,
                             unsigned int writeOutFrequency,
                             std::string filename)
    : endTime(endTime),
      deltaT(deltaT),
      writeOutFrequency(writeOutFrequency),
      filename(std::move(filename)) {}

double SimulationArg::getEndTime() const { return endTime; }

double SimulationArg::getDeltaT() const { return deltaT; }

unsigned int SimulationArg::getWriteOutFrequency() const {
  return writeOutFrequency;
}

const std::string& SimulationArg::getFilename() const { return filename; }

SimulationArg::~SimulationArg() {}
