#include "LineProcessorCheckpoint.h"

#include <array>

#include "spdlog/spdlog.h"
using v3d = std::array<double, 3>;

void LineProcessorCheckpoint::processLine(std::istringstream &s, IContainer &container) {
  Particle particle{};

  // Changing the order of the following lines, changes the order in which values are read from the input file
  particle.x = getNextArray<3>(s);
  particle.v = getNextArray<3>(s);
  particle.old_f = getNextArray<3>(s);
  particle.f = getNextArray<3>(s);
  particle.m = getNext(s);
  particle.type = {static_cast<int>(getNext(s))};
  particle.epsilon = getNext(s);
  particle.sigma = getNext(s);

  container.push_back(particle);
}

double LineProcessorCheckpoint::getNext(std::istringstream &dataStream) {
  double value{};

  if (dataStream.eof()) {
    spdlog::error("Checkpointing file malformed. Exiting program.");
    exit(-1);
  }

  dataStream >> value;

  return value;
}

template <std::size_t size>
std::array<double, size> LineProcessorCheckpoint::getNextArray(std::istringstream &dataStream) {
  std::array<double, size> values;
  for (auto &component : values) component = getNext(dataStream);
  return values;
}
