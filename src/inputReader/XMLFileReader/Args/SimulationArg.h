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

  [[nodiscard]] double getEndTime() const;

  [[nodiscard]] double getDeltaT() const;

  [[nodiscard]] unsigned int getWriteOutFrequency() const;

  [[nodiscard]] const std::string& getFilename() const;

  [[nodiscard]] std::string getInputFile() const;

  virtual ~SimulationArg();
};