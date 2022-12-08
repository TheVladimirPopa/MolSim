#include "SimulationArg.h"

#include <utility>

double SimulationArg::getEndTime() const { return endTime; }

double SimulationArg::getDeltaT() const { return deltaT; }

unsigned int SimulationArg::getWriteOutFrequency() const {
  return writeOutFrequency;
}

const std::string& SimulationArg::getFilename() const { return filename; }

SimulationArg::~SimulationArg() {}

std::string SimulationArg::getInputFile() const { return inputFile; }

SimulationArg::SimulationArg(double endTime, double deltaT,
                             unsigned int writeOutFrequency,
                             std::string filename, std::string inputFile) {}
