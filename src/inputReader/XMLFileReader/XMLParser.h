#pragma once
#include <memory>

#include "Simulation.h"
#include "dataStructures/LinkedCellsBoundary.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "inputReader/XMLFileReader/Args/CuboidArg.h"
#include "inputReader/XMLFileReader/Args/LinkedCellArg.h"
#include "inputReader/XMLFileReader/Args/MembraneArg.h"
#include "inputReader/XMLFileReader/Args/ParticleArg.h"
#include "inputReader/XMLFileReader/Args/SimulationArg.h"
#include "inputReader/XMLFileReader/Args/SphereArg.h"
#include "inputReader/XMLFileReader/Args/ThermostatArg.h"
#include "inputReader/XMLFileReader/XMLFiles/input.h"
#include "utils/MolSimEnums.h"
#include "utils/SimulationUtils.h"

class XMLParser {
 private:
  std::unique_ptr<simulation_t> simulation;
  /**
   * Generates an array of type double from the xml input
   * @param input
   * @return
   */
  static std::array<double, 3> generate_double_array(const array_d &input) { return {input.x(), input.y(), input.z()}; }
  /**
   * Generates an array of type int from the xml input
   * @param input
   * @return
   */
  static std::array<int, 3> generate_int_array(const array_i &input) { return {input.x(), input.y(), input.z()}; }

 public:
  /**
   * Generates an XMLParser from a given path. If the path is not valid, an
   * error is thrown
   * @param path
   */
  explicit XMLParser(const std::string &path) {
    try {
      simulation = Simulation_XML(path, xml_schema::flags::dont_validate);
    } catch (const xml_schema::exception &e) {
      throw std::invalid_argument(std::string{e.what()});
    }
  }
  /**
   * Extracts the arguments (endTime, deltaT, writeOutFrequency and filename)
   * used for a simulation from the XML file
   * @return Returns a SimulationArg
   */
  SimulationArg extractSimulation() {
    std::vector<std::string> files;

    for (auto &e : simulation->InputFile()) {
      std::string path = e.path();
      files.push_back(path);
    }

    double endTime = simulation->endTime();
    double deltaT = simulation->deltaT();
    unsigned int writeOutFrequency = simulation->writeOutFrequency();
    std::string filename = simulation->filename();

    SimulationArg arg = SimulationArg(endTime, deltaT, writeOutFrequency, filename, files.at(0));

    return arg;
  }
  /**
   * Converts BoundaryType string str to proper BoundaryType enum
   * @param str
   * @return Returns a BoundaryType corresponding to the string
   */
  static BoundaryType strToEnumBoundary(const std::string &str) {
    if (str == "OUTFLOW") {
      return BoundaryType::OUTFLOW;
    } else if (str == "REFLECT") {
      return BoundaryType::REFLECT;
    } else if (str == "PERIODIC") {
      return BoundaryType::PERIODIC;
    }
    throw std::invalid_argument("Invalid boundary string");
  }

  /**
   * @return Returns type of the container specified in the xml file
   */
  ContainerType getContainerType() {
    for (auto &it : simulation->Container_T()) {
      if (it.LinkedCell().empty()) return ::ContainerType::VECTOR;
    }
    return ContainerType::LINKED_CELLS;
  }
  /**
   * Extracts the arguments (boundaries, cellSize) used to initialise a
   * LinkedCellContainer from the XML file
   * @return Returns a LinkedCellArg
   */
  LinkedCellArg extractLinkedCell() {
    for (auto &it : simulation->Container_T()) {
      for (auto &c : it.LinkedCell()) {
        auto lb = c.leftLowerBound();
        auto rb = c.rightUpperBound();
        auto cs = c.cellSize();
        auto left = c.left();
        auto right = c.right();
        auto top = c.top();
        auto bottom = c.bottom();
        auto front = c.front();
        auto back = c.back();
        auto para = c.parallelization();

        LinkedCellArg linkedCellArg = LinkedCellArg(cs, generate_double_array(lb), generate_double_array(rb), para);
        linkedCellArg.setBoundLeft(left);
        linkedCellArg.setBoundRight(right);
        linkedCellArg.setBoundTop(top);
        linkedCellArg.setBoundBottom(bottom);
        linkedCellArg.setBoundFront(front);
        linkedCellArg.setBoundBack(back);
        return linkedCellArg;
      }
    }

    throw std::invalid_argument("XML Parser found no linked cells container");
  }
  /**
   * Extracts the arguments (position, velocity, dimension, distance, mass,
   * type) used to initialise cuboids from the XML file
   * @return Returns a vector of CuboidArg
   */
  [[nodiscard]] std::vector<CuboidArg> extractCuboid() const {
    std::vector<CuboidArg> cuboidArgs;

    for (auto &it : simulation->SimTypes()) {
      for (auto &s : it.Cuboid()) {
        auto pos = s.position();
        auto vel = s.velocity();
        auto dim = s.dimension();
        auto dis = s.distance();
        auto mass = s.mass();
        auto type = s.type();

        cuboidArgs.emplace_back(generate_double_array(pos), generate_double_array(vel), generate_int_array(dim), dis,
                                mass, type);
      }
    }

    return cuboidArgs;
  }
  /**
   * Extracts the arguments (position, velocity, radius, distance, mass,
   * dimension, type) used to initialise spheres from the XML file
   * @return Returns a vector of SphereArg
   */
  [[nodiscard]] std::vector<SphereArg> extractSpheres() const {
    std::vector<SphereArg> sphereArgs;

    for (auto &it : simulation->SimTypes()) {
      for (auto &s : it.Sphere()) {
        auto pos = s.position();
        auto vel = s.velocity();
        auto rad = s.radius();
        auto dis = s.distance();
        auto mass = s.mass();
        auto dim = s.dimension();
        auto type = s.type();

        sphereArgs.emplace_back(generate_double_array(pos), generate_double_array(vel), rad, dim, dis, mass, type);
      }
    }

    return sphereArgs;
  }

  std::vector<MembraneArg> extractMembrane() {
    std::vector<MembraneArg> args;
    for (auto &it : simulation->Membrane()) {
      auto pos = it.position();
      auto dim = it.dimension();
      auto vel = it.velocity();
      auto dist = it.distance();
      auto mass = it.mass();
      auto stiff = it.stiffness();
      auto bl = it.bondLength();
      auto cor = it.cutOffRadius();
      auto type = it.type();

      args.emplace_back(generate_double_array(pos), generate_int_array(dim), generate_double_array(vel),
                        extractMembraneForces(), dist, mass, stiff, bl, cor, type);
    }
    return args;
  }

  std::vector<MembraneForceArg> extractMembraneForces() {
    std::vector<MembraneForceArg> membraneForces;
    for (auto &it : simulation->Membrane()) {
      for (auto &mf : it.membraneForce()) {
        auto row = mf.row();
        auto col = mf.column();
        auto x = mf.x();
        auto y = mf.y();
        auto z = mf.z();
        auto ts = mf.timeSpan();

        membraneForces.emplace_back(row, col, x, y, z, ts);
      }
    }

    return membraneForces;
  }
  /**
   * Extracts the arguments (initTemp, targetTemp, maxTempChange, periodLength, dimension) used to initialise a
   * Thermostat from the XML file
   * @return Returns a ThermostatArg
   */
  ThermostatArg getThermostat() {
    for (auto &it : simulation->Thermostat()) {
      auto initialTemp = it.initialTemp();
      auto targetTemp = it.targetTemp();
      auto maxTempChange = it.maxTempChange();
      auto periodLength = it.periodLength();
      auto dimension = it.dimension();

      ThermostatArg thermostatArg{initialTemp, targetTemp, maxTempChange, periodLength, dimension};
      return thermostatArg;
    }
    throw std::invalid_argument("XML Parser found no thermostat");
  }
  /**
   * Extracts the arguments (id, epsilon, sigma) used to initialise particles from the XML file
   * @return Returns a vector of ParticleArg
   */
  [[nodiscard]] std::vector<ParticleArg> extractParticles() const {
    std::vector<ParticleArg> particleArgs;

    for (auto &it : simulation->Particle()) {
      particleArgs.emplace_back(it.id(), it.epsilon(), it.sigma());
    }

    return particleArgs;
  }
  /**
   * Initialises the given simulation sim with the corresponding arguments from
   * the path file
   * @param sim
   */
  void initialiseSimulationFromXML(Simulation &sim) {
    SimulationArg args = extractSimulation();
    sim.setDeltaT(args.getDeltaT());
    sim.setEndTime(args.getEndTime());
    sim.setIterationsPerWrite(args.getWriteOutFrequency());
    sim.setFilename(args.getFilename());
  }
  /**
   * Adds all cuboids read from the path file to a given LinkedCellsContainer
   * @param linkedCellsContainer
   */
  [[nodiscard]] std::vector<ParticleGeneration::cuboid> getCuboids() const {
    std::vector<ParticleGeneration::cuboid> cuboids;
    std::vector<CuboidArg> cuboidArgs = this->extractCuboid();

    for (auto &it : cuboidArgs) {
      ParticleGeneration::cuboid cuboid;
      cuboid.position = it.getPosition();
      cuboid.dimension = it.getDimension();
      cuboid.velocity = it.getVelocity();
      cuboid.distance = it.getDistance();
      cuboid.mass = it.getMass();
      cuboid.type = it.getType();
      cuboids.push_back(cuboid);
    }

    return cuboids;
  }

  /**
   * Return path where to load the checkpoint from
   * @param checkpoint
   */
  std::string getCheckpointPath() {
    SimulationArg args = extractSimulation();
    return args.getInputFile();
  }
  /**
   * Adds all spheres read from the path file to a given Container
   * @param linkedCellsContainer
   */
  [[nodiscard]] std::vector<ParticleGeneration::sphere> getSpheres() const {
    std::vector<ParticleGeneration::sphere> spheres;

    std::vector<SphereArg> sphereArgs = extractSpheres();
    for (auto &it : sphereArgs) {
      ParticleGeneration::sphere sphere;
      sphere.position = it.getPosition();
      sphere.radius = it.getRadius();
      sphere.dimension = it.getDimension();
      sphere.velocity = it.getVelocity();
      sphere.distance = it.getDistance();
      sphere.mass = it.getMass();
      sphere.type = it.getType();
      spheres.push_back(sphere);
    }

    return spheres;
  }

  /**
   * @return Returns the Lennard Jones cutOffRadius of the simulation
   */
  [[nodiscard]] double getCutOffRadius() const { return simulation->cutOffRadius(); }

  /**
   * Reads membranes from xml file
   * @return Extracted membranes
   */
  std::vector<ParticleGeneration::membrane> getMembranes() {
    std::vector<ParticleGeneration::membrane> membranes;
    std::vector<MembraneArg> membraneArgs = extractMembrane();

    for (auto &it : membraneArgs) {
      ParticleGeneration::membrane membrane;
      membrane.position = it.getPosition();
      membrane.dimension = it.getDimension();
      membrane.velocity = it.getVelocity();
      membrane.distance = it.getDistance();
      membrane.mass = it.getMass();
      membrane.type = it.getType();
      membrane.bondLength = it.getBondLength();
      membrane.stiffness = it.getStiffness();
      membrane.cutOffRadius = it.getCutOffRadius();
      std::vector<ParticleGeneration::MembraneForce> membraneForces;
      for (MembraneForceArg mfa : it.getMembraneForces()) {
        ParticleGeneration::MembraneForce membraneForce;
        membraneForce.row = mfa.getRow();
        membraneForce.column = mfa.getColumn();
        membraneForce.x = mfa.getX();
        membraneForce.y = mfa.getY();
        membraneForce.z = mfa.getZ();
        membraneForce.timeSpan = mfa.getTimeSpan();
        membraneForces.emplace_back(membraneForce);
      }
      membrane.membraneForces = membraneForces;
      membranes.push_back(membrane);
    }

    return membranes;
  }
  /**
   * @return Returns the gravity constant
   */
  [[nodiscard]] double getGravityConstant() const { return simulation->gravity(); }

  /**
   * @return The selected model
   */
  [[nodiscard]] ModelType getModel() const {
    // TODO: Read this value from "simulation" once the xml part is updated
    return ModelType::LennardJones;
  }

  /**
   * Registers the particles read in the path file
   */
  void initializeParticleTypes() const {
    std::vector<ParticleArg> particleArgs = extractParticles();
    for (auto &p : particleArgs) {
      Particle::registerParticleType(p.getId(), p.getEpsilon(), p.getSigma());
    }
  }
  /**
   * Constructs a LinkedCellsContainer from the path file if that container type is chosen
   * @return Returns a LinkedCellsContainer
   */
  std::unique_ptr<LinkedCellsContainer> initialiseLinkedCellContainerFromXML() {
    LinkedCellArg linkedCellArg = extractLinkedCell();
    std::array<double, 3> lowerBound = linkedCellArg.getLeftLowerBound();
    std::array<double, 3> upperBound = linkedCellArg.getRightUpperBound();
    std::string parallelization = linkedCellArg.getParallelization();

    // TODO initialise the parallelization

    auto container = std::make_unique<LinkedCellsContainer>(linkedCellArg.getCellSize(), lowerBound, upperBound);
    applyBoundariesFromXML(*container);
    return container;
  }

  /**
   * Constructs a thermostat from the path file and a given container
   * @param container
   * @return Returns a Thermostat
   */
  std::unique_ptr<Thermostat> initialiseThermostatFromXML(IContainer &container) {
    ThermostatArg thermostatArg = getThermostat();
    size_t dimension = thermostatArg.getDimension();
    return std::make_unique<Thermostat>(container, thermostatArg.getInitialTemp(), thermostatArg.getTargetTemp(),
                                        thermostatArg.getMaxTempChange(), thermostatArg.getPeriodLength(), dimension);
  }
  /**
   *  Sets boundaries read from the path file to a given LinkedCellContainer
   * @param linkedCellsContainer
   */
  void applyBoundariesFromXML(LinkedCellsContainer &linkedCellsContainer) {
    LinkedCellArg linkedCellArg = extractLinkedCell();
    linkedCellsContainer.setBoundaries({
        {CubeSide::LEFT, XMLParser::strToEnumBoundary(linkedCellArg.getBoundLeft())},
        {CubeSide::RIGHT, XMLParser::strToEnumBoundary(linkedCellArg.getBoundRight())},
        {CubeSide::TOP, XMLParser::strToEnumBoundary(linkedCellArg.getBoundTop())},
        {CubeSide::BOTTOM, XMLParser::strToEnumBoundary(linkedCellArg.getBoundBottom())},
        {CubeSide::FRONT, XMLParser::strToEnumBoundary(linkedCellArg.getBoundFront())},
        {CubeSide::BACK, XMLParser::strToEnumBoundary(linkedCellArg.getBoundBack())},
    });
  }

  double getEndTime() { return simulation->endTime(); }
  double getDeltaT() { return simulation->deltaT(); }
  double getWriteInterval() { return simulation->writeOutFrequency(); }
};
