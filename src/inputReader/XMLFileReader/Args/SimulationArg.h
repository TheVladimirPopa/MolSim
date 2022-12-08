#pragma once
#include <iostream>
#include <string>

class SimulationArg {
 private:
  double endTime{};
  double deltaT{};
  unsigned int writeOutFrequency{};
  std::string filename;
  std::string inputFile;

 public:
  SimulationArg(double endTime, double deltaT, unsigned int writeOutFrequency,
                std::string  filename, std::string inputFile);

  double getEndTime() const;
  double getDeltaT() const;
  unsigned int getWriteOutFrequency() const;
  const std::string& getFilename() const;
  std::string getInputFile() const;

  virtual ~SimulationArg();
};