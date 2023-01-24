#include "StatisticsWriter.h"
#include "utils/ThermodynamicStatistics.h"
StatisticsWriter::StatisticsWriter() = default;
void StatisticsWriter::writeFile(const std::string& filename, int iteration, IContainer& particles) {
  file.open(filename, std::ios::out);

  file.flush();
  file.close();
}
