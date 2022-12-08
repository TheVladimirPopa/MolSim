/*
 * FileReader.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "FileReader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "spdlog/spdlog.h"

FileReader::FileReader() = default;

FileReader::~FileReader() = default;

void FileReader::readFile(IContainer &container, char *filename,
                          ILineProcessor &lineProcessor) {
  int num_particles = 0;

  std::ifstream input_file(filename);
  std::string tmp_string;

  if (input_file.is_open()) {
    spdlog::info("Reading from file {}", filename);
    getline(input_file, tmp_string);

    while (tmp_string.empty() or tmp_string[0] == '#') {
      getline(input_file, tmp_string);
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    spdlog::debug("Reading in {} particle (groups)", num_particles);
    getline(input_file, tmp_string);

    container.reserve(num_particles);
    for (int i = 0; i < num_particles; i++) {
      std::istringstream dataStream(tmp_string);

      lineProcessor.processLine(dataStream, container);
      getline(input_file, tmp_string);
    }
  } else {
    spdlog::error("Error: could not open {}", filename);
    exit(-1);
  }
}
