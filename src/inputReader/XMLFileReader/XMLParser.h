//
// Created by vladi on 11/27/2022.
//
#pragma once
#include <memory>

#include "Simulation.h"
#include "XMLParser.h"
#include "inputReader/XMLFileReader/Args/CuboidArg.h"
#include "inputReader/XMLFileReader/Args/LinkedCellArg.h"
#include "inputReader/XMLFileReader/Args/SimulationArg.h"
#include "inputReader/XMLFileReader/Args/SphereArg.h"
#include "inputReader/XMLFileReader/XMLFiles/input.h"

class XMLParser {
 private:
  /**
   * XML file simulation
   */
  std::unique_ptr<simulation_t> simulation;

  /**
   * Generates an array of type double from the xml input
   * @param input
   * @return
   */

  static std::array<double, 3> generate_double_array(const array_d &input) {
    return {input.x(), input.y(), input.z()};
  }

  /**
   * Generates an array of type int from the xml input
   * @param input
   * @return
   */

  static std::array<int, 3> generate_int_array(const array_i &input) {
    return {input.x(), input.y(), input.z()};
  }

 public:
  explicit XMLParser(const std::string &path) {
    try {
      simulation = Simulation_XML(path, xml_schema::flags::dont_validate);
    } catch (const xml_schema::exception &e) {
      throw std::invalid_argument(std::string{e.what()});
    }
  }

  std::unique_ptr<SimulationArg> extractSimulation() {
    std::vector<std::string> files;

    for (auto &e : simulation->InputFile()) {
      std::string path = e.path();
      files.push_back(path);
    }

    double endTime = simulation->endTime();
    double deltaT = simulation->deltaT();
    unsigned int writeOutFrequency = simulation->writeOutFrequency();
    std::string filename = simulation->filename();

    SimulationArg arg =
        SimulationArg(endTime, deltaT, writeOutFrequency, filename, files[0]);

    return std::make_unique<SimulationArg>(arg);
  }

  static boundaryType strToEnumBoundary(const std::string &str) {
    if (str == "OUTFLOW") {
      return boundaryType::OUTFLOW;
    } else if (str == "REFLECT") {
      return boundaryType::REFLECT;
    } else if (str == "PERIODIC") {
      return boundaryType::PERIODIC;
    }
    throw std::errc::invalid_argument;
  }

  std::unique_ptr<LinkedCellArg> extractLinkedCell() {
    std::vector<LinkedCellArg> arg;

    for (auto &it : simulation->Container_T()) {
      for (auto &c : it.LinkedCell()) {
        auto &lb = c.leftLowerBound();
        auto &rb = c.rightUpperBound();
        auto &cs = c.cellSize();
        auto &cut = c.cutOffRadius();
        auto &left = c.left();
        auto &right = c.right();
        auto &top = c.top();
        auto &bottom = c.bottom();
        auto &front = c.front();
        auto &back = c.back();

        LinkedCellArg linkedCellArg = LinkedCellArg(
            cs, cut, generate_double_array(lb), generate_double_array(rb));
        linkedCellArg.setBoundLeft(left);
        linkedCellArg.setBoundRight(right);
        linkedCellArg.setBoundTop(top);
        linkedCellArg.setBoundBottom(bottom);
        linkedCellArg.setBoundFront(front);
        linkedCellArg.setBoundBack(back);
        arg.emplace_back(linkedCellArg);
      }
    }

    return std::make_unique<LinkedCellArg>(arg[0]);
  }

  [[nodiscard]] std::vector<CuboidArg> extractCuboid() const {
    std::vector<CuboidArg> cuboidArgs;

    for (auto &it : simulation->SimTypes()) {
      for (auto &s : it.Cuboid()) {
        auto &pos = s.position();
        auto &vel = s.velocity();
        auto &dim = s.dimension();
        auto &dis = s.distance();
        auto &mass = s.mass();
        auto &type = s.type();

        cuboidArgs.emplace_back(generate_double_array(pos),
                                generate_double_array(vel),
                                generate_int_array(dim), dis, mass, type);
      }
    }

    return cuboidArgs;
  }

  [[nodiscard]] std::vector<SphereArg> extractSpheres() const {
    std::vector<SphereArg> sphereArgs;

    for (auto &it : simulation->SimTypes()) {
      for (auto &s : it.Sphere()) {
        auto &pos = s.position();
        auto &vel = s.velocity();
        auto &rad = s.radius();
        auto &dis = s.distance();
        auto &mass = s.mass();
        auto &dim = s.dimension();
        auto &type = s.type();

        sphereArgs.emplace_back(generate_double_array(pos),
                                generate_double_array(vel), rad, dim, dis, mass,
                                type);
      }
    }

    return sphereArgs;
  }

  void initialiseSimulationFromXML(Simulation &sim) {
    std::unique_ptr<SimulationArg> args = this->extractSimulation();
    sim.setDeltaT(args->getDeltaT());
    sim.setEndTime(args->getEndTime());
    sim.setIterationsPerWrite(args->getWriteOutFrequency());
    sim.setFilename(args->getFilename());
  }

  void initialiseCuboidsFromXML(
      LinkedCellsContainer &linkedCellsContainer) const {
    std::vector<CuboidArg> cuboidArgs = this->extractCuboid();

    for (auto &it : cuboidArgs) {
      ParticleGeneration::cuboid cuboid;
      cuboid.position = it.getPosition();
      cuboid.dimension = it.getDimension();
      cuboid.velocity = it.getVelocity();
      cuboid.distance = it.getDistance();
      cuboid.mass = it.getMass();
      cuboid.type = it.getType();
      ParticleGeneration::addCuboidToParticleContainer(linkedCellsContainer,
                                                       cuboid);
    }
  }

  void initialiseSpheresFromXML(
      LinkedCellsContainer &linkedCellsContainer) const {
    std::vector<SphereArg> sphereArgs = this->extractSpheres();
    for (auto &it : sphereArgs) {
      ParticleGeneration::sphere sphere;
      sphere.position = it.getPosition();
      sphere.radius = it.getRadius();
      sphere.dimension = it.getDimension();
      sphere.velocity = it.getVelocity();
      sphere.distance = it.getDistance();
      sphere.mass = it.getMass();
      sphere.type = it.getType();
      ParticleGeneration::addSphereToParticleContainer(linkedCellsContainer,
                                                       sphere);
    }
  }

  LinkedCellsContainer initialiseLinkedCellContainerFromXML() {
    std::unique_ptr<LinkedCellArg> linkedCellArg = this->extractLinkedCell();
    std::array<double, 3> lowerBound = linkedCellArg->getLeftLowerBound();
    std::array<double, 3> upperBound = linkedCellArg->getRightUpperBound();

    return LinkedCellsContainer{linkedCellArg->getCellSize(), lowerBound,
                                upperBound};
  }

  double initCutOffRadiusXML() {
    std::unique_ptr<LinkedCellArg> linkedCellArg = this->extractLinkedCell();
    return linkedCellArg->getCutOffRadius();
  }

  void XMLLinkedCellBoundaries(LinkedCellsContainer &linkedCellsContainer) {
    std::unique_ptr<LinkedCellArg> linkedCellArg = this->extractLinkedCell();
    linkedCellsContainer.setBoundaries({
        {cubeSide::LEFT,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundLeft())},
        {cubeSide::RIGHT,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundRight())},
        {cubeSide::TOP,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundTop())},
        {cubeSide::BOTTOM,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundBottom())},
        {cubeSide::FRONT,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundFront())},
        {cubeSide::BACK,
         XMLParser::strToEnumBoundary(linkedCellArg->getBoundBack())},
    });
  }
};
