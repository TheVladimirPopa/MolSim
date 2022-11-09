//
// Created by leo on 09.11.22.
//
#pragma once
#include "dataStructures/Particle.h"
#include "dataStructures/ParticleContainer.h"
#include "inputReader/FileReader.h"
#include "model/IModel.h"
#include "model/NewtonsLawModel.h"
#include "outputWriter/IWriter.h"
#include "outputWriter/VTKWriter.h"

class Simulation {
 public:
  /**
   * Simulate the particles with the given model and stepwidth of deltaT
   * The startTime and endTime variable are used to determine the duration of
   * the simulation
   * @param model Defining the model to update x, f and v in each iteration
   * @param particles All the particles that take part in the simulation
   * @param fileWriter A way of outputting the results every nth iterations to a
   * file
   */
  void simulate(IModel const &model, ParticleContainer &particles,
                IWriter &fileWriter);

  void setStartTime(double startTime);
  void setEndTime(double endTime);
  void setDeltaT(double deltaT);
  void setWriteOutFrequency(int writeOutFrequency);
  void setFilename(const std::string &filename);

 private:
  /**
   * The starttime where the simulation begins
   */
  double startTime = 0;
  /**
   * The time until the simulation is run (default 1000)
   */
  double endTime = 1000;
  /**
   *  The timestep by which the time gets increased in every iteration (default
   * 0.014)
   */
  double deltaT = 0.014;
  /**
   * States how often the particles get written to a file (every nth
   * iteration)(default 10)
   */
  int writeOutFrequency = 10;
  /**
   * Path to output file (default "MD_vtk")
   */
  std::string filename = "MD_vtk";
};
