#pragma once
#include <iostream>
#include <string>

class SimulationArg {
 private:
  double endTime{};
  double deltaT{};
  unsigned int writeOutFrequency{};
  std::string filename;

 public:
  SimulationArg(double endTime, double deltaT, unsigned int writeOutFrequency,
                std::string  filename);

  double getEndTime() const;
  double getDeltaT() const;
  unsigned int getWriteOutFrequency() const;
  const std::string& getFilename() const;

  virtual ~SimulationArg();
};
