//
// Created by vladi on 11/27/2022.
//
#pragma once
#include <memory>

#include "CuboidArg.h"
#include "Simulation.h"
#include "SimulationArg.h"
#include "SphereArg.h"
#include "XMLParser.h"
#include "input.hxx"

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

  std::array<double, 3> generate_double_array(const array_d &input) const {
    return {input.x(), input.y(), input.z()};
  }

  /**
   * Generates an array of type int from the xml input
   * @param input
   * @return
   */

  std::array<int, 3> generate_int_array(const array_i &input) const {
    return {input.x(), input.y(), input.z()};
  }

  std::vector<SphereArg> extractSpheres() const {
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

  std::vector<CuboidArg> extractCuboid() const {
    std::vector<CuboidArg> cuboidArgs;
    CuboidArg arg{};

    for (auto &it : simulation->SimTypes()) {
      for (auto &s : it.Cuboid()) {
        auto &pos = s.position();
        auto &vel = s.velocity();
        auto &dim = s.dimension();
        auto &dis = s.distance();
        auto &mass = s.mass();
        auto &type = s.type();

        //        cuboidArg.emplace_back(generate_double_array(pos),
        //                               generate_double_array(vel),
        //                               generate_int_array(dim), dis, mass,
        //                               dim, type);
        arg = CuboidArg(generate_double_array(pos), generate_double_array(vel),
                   generate_int_array(dim), dis, mass, type);
        cuboidArgs.emplace_back(arg);
      }
    }

    return cuboidArgs;
  }

 public:
  XMLParser(const std::string &path) {
    try {
      simulation = Simulation(path, xml_schema::flags::dont_validate);
    } catch (const xml_schema::exception &e) {
      throw std::invalid_argument(std::string{e.what()});
    }
  }

  std::unique_ptr<SimulationArg> parseXMl() {
    std::vector<std::string> files;

    for (auto &e : simulation->InputFile()) {
      std::string path = e.path();
      files.push_back(path);
    }

    double endTime = simulation->endTime();
    double deltaT = simulation->deltaT();
    int writeOutFrequency = simulation->writeOutFrequency();
    std::string filename = simulation->filename();
  }

  // return simulation attributes somehow
};
