#pragma once

#include <variant>

#include "MolSimEnums.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "inputReader/XMLFileReader/XMLParser.h"
#include "outputWriter/NoWriter.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using duration = std::chrono::nanoseconds;
using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;
using ParticleShape = std::variant<ParticleGeneration::sphere, ParticleGeneration::cuboid>;

namespace SimulationUtils {

/**
 * Makes a container that can hold particles
 * @param type Type of the container (vector or linked cells)
 * @param container A container that can be returned (this is a placeholder to stream line the interface for now)
 * @return The container
 */
std::unique_ptr<IContainer> makeContainer(ContainerType type, std::unique_ptr<IContainer> container);

/**
 * Makes a default container. The container is a hard coded linked cells container.
 * @return Hard coded linked cells container
 */
std::unique_ptr<LinkedCellsContainer> makeDefaultContainer();

/**
 * Adds individual particles to the container.
 * @param container The container that receives particles
 * @param shapes The shapes (cuboids, spheres, etc.) that will be converted into individual particles
 */
void populateContainer(IContainer &container, std::vector<ParticleShape> shapes);

/**
 * Reads file input and based on the type adds particles to the container. (e.g. from a sphere file)
 * @param container The container that receives particles
 * @param filePath The path to the file
 * @param type The type of the file (e.g. cuboid, sphere)
 * @note This type of input is deprecated.
 */
void populateContainerViaFile(IContainer &container, std::string filePath, SimTypeDeprecated type);

/**
 * Reads a checkpoint file and inputs the stored particles into the container.
 * @param container The container which receives the checkpointed particles
 * @param checkpointPath The path to the checkpoint file
 */
void loadCheckpoint(IContainer &container, std::string checkpointPath);

/**
 * Makes the defined model
 * @param modelType The type of the model (e.g. lennard-jones)
 * @param deltaT The delta_t which is used by the model
 * @param cutOff The cutoff radius which is used by the model
 * @return The prepared model
 */
std::unique_ptr<IModel> makeModel(ModelType modelType, double deltaT, double cutOff = 1e9);

/**
 * Makes a writer that can output the contents of a container to a file
 * @param writerType Type of the writer (e.g. vtk writer)
 * @return The writer
 */
std::unique_ptr<IWriter> makeWriter(WriterType writerType);

/**
 * Makes a default, hardcoded thermostat.
 * @param container The container that receives the thermostat
 * @return The thermostat
 */
std::unique_ptr<Thermostat> makeDefaultThermostat(IContainer &container);

/**
 * Makes a new thermostat
 * @param thermostat A thermostat that will be returned. (This is a placeholder to streamline the interface for now)
 * @return The thermostat
 */
std::unique_ptr<Thermostat> makeThermostat(std::unique_ptr<Thermostat> thermostat);

}  // namespace SimulationUtils

namespace ConfigurationUtils {

/**
 * Sets the global logging behaviour of the program.
 * @param logLevel Level 0: Print info and error messages. Level 1: Print debug and level 0 messages. Level 2: Print
 * trace and level 1 messages
 * @param disableLogging If true, only errors will be printed. Errors also normally cause the simulation to be
 * terminated.
 */
void configureLogging(int logLevel, bool disableLogging);

/**
 * Prints the measured performance of the simulation
 * @param startTime Time when simulation started. (Excludes loading of files)
 * @param endTime Time when simulation ended.
 * @param simulation The simulation object which stores the performance information.
 */
void printPerformanceMeasure(time_point startTime, time_point endTime, Simulation &simulation);

/**
 * Prints the hitrate of the model.
 * Hitrate is defined as (number_of_actual_calculations)/(number_of_total_calculations) where actual_calculations are
 * the calculations where two particles are within the cutoff radius. And total_calculations are calculations ignoring
 * the cutoff radius.
 * @param model The model that was used to perform the calculations.
 */
void printHitrateMeasure(std::unique_ptr<IModel> model);
}  // namespace ConfigurationUtils
