//
// Created by leo on 09.11.22.
//
#pragma once
#include "dataStructures/IContainer.h"
#include "dataStructures/Particle.h"
#include "inputReader/FileReader.h"
#include "model/IModel.h"
#include "model/NewtonsLawModel.h"
#include "model/Thermostat.h"
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
   * @param thermostat The thermostat that controls the temperature
   * @param gravitationalConstant The factor of the gravitational force that gets applied
   * to a particle every iteration
   */
  void simulate(IModel &model, IContainer &particles, IWriter &fileWriter, Thermostat &thermostat,
                double gravitationalConstant = 0.0);

  /**
   * Sets the time until the simulation is run
   * @param endTime the new endTime
   */
  void setEndTime(double endTime) { Simulation::endTime = endTime; }
  /**
   * Sets the timestep by which the time gets increased in every iteration
   * @param deltaT the new timestep size
   */
  void setDeltaT(double deltaT) { Simulation::deltaT = deltaT; }
  /**
   * Sets how often the particles get written to a file (every nth iteration)
   * @param writeOutFrequency the new frequency for the writer
   */
  void setIterationsPerWrite(unsigned int writeOutFrequency) { Simulation::writeOutFrequency = writeOutFrequency; }
  /**
   * Sets the path to the output files(iteration number and file-ending are
   * added automatically)
   * @param filename the new path
   */
  void setFilename(const std::string &filename) { Simulation::filename = filename; }

  /**
   * Returns the timestep by which the time gets increased in every iteration
   * @return The timestep by which the time gets increased in every iteration
   */
  double getDeltaT() const { return deltaT; };

  /**
   * Returns the time until the simulation is run
   * @return The time until the simulation is run
   */
  double getEndTime() const { return endTime; };
  /**
   * Returns the frequency of file writing
   * @return the frequency of file writing
   */
  unsigned int getWriteOutFrequency() const { return writeOutFrequency; }
  /**
   * Returns the prefix of the output files
   * @return the prefix of the output files
   */
  const std::string getFilename() const { return filename; }

  /**
   * Returns the total position updates of all particles
   * @return the total position updates of all particles
   */
  size_t getMoleculeUpdateCount() const { return moleculeUpdateCount; }

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
  unsigned int writeOutFrequency = 10;
  /**
   * Path to output file (default "MD_vtk")
   */
  std::string filename = "MD_vtk";

  /**
   * Counts the total position updates of all particles
   */
  size_t moleculeUpdateCount = 0;
};
