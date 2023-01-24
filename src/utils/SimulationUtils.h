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

std::unique_ptr<LinkedCellsContainer> makeDefaultContainer();

std::unique_ptr<IContainer> makeContainer(ContainerType type, std::unique_ptr<IContainer> container);

void populateContainerViaFile(IContainer &container, std::string filePath, SimTypeDeprecated type);

void populateContainer(IContainer &container, bool loadCheckpoint, std::vector<ParticleShape> shapes);

void loadCheckpoint(IContainer &container, std::string checkpointPath);

std::unique_ptr<IModel> makeModel(ModelType modelType, double deltaT, double cutOff = 1e9);

std::unique_ptr<IWriter> makeWriter(WriterType writerType);

std::unique_ptr<Thermostat> makeDefaultThermostat(IContainer& container);

std::unique_ptr<Thermostat> makeThermostat(std::unique_ptr<Thermostat> thermo);

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
