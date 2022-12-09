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

  std::unique_ptr<LinkedCellArg> extractLinkedCell() {
    std::vector<LinkedCellArg> arg;

    for (auto &it : simulation->Container_T()) {
      for (auto &c : it.LinkedCell()) {
        auto &lb = c.leftLowerBound();
        auto &rb = c.rightUpperBound();
        auto &cs = c.cellSize();
        auto &cut = c.cutOffRadius();

        LinkedCellArg linkedCellArg = LinkedCellArg(
            cs, cut, generate_double_array(lb), generate_double_array(rb));

        arg.emplace_back(cs, cut, generate_double_array(lb),
                         generate_double_array(rb));
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
};
