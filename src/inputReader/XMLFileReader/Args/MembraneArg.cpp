#include "MembraneArg.h"

#include <utility>
MembraneArg::MembraneArg(const std::array<double, 3>& position, const std::array<int, 3>& dimension,
                         const std::array<double, 3>& velocity, std::vector<MembraneForceArg> membraneForces,
                         double distance, double mass, double stiffness, double bondLength, double cutOffRadius,
                         int type)
    : position(position),
      dimension(dimension),
      velocity(velocity),
      membraneForces(std::move(membraneForces)),
      distance(distance),
      mass(mass),
      stiffness(stiffness),
      bondLength(bondLength),
      cutOffRadius(cutOffRadius),
      type(type) {}

const std::array<double, 3>& MembraneArg::getPosition() const { return position; }

const std::array<int, 3>& MembraneArg::getDimension() const { return dimension; }

const std::array<double, 3>& MembraneArg::getVelocity() const { return velocity; }

const std::vector<MembraneForceArg>& MembraneArg::getMembraneForces() const { return membraneForces; }

double MembraneArg::getDistance() const { return distance; }

double MembraneArg::getMass() const { return mass; }

double MembraneArg::getStiffness() const { return stiffness; }

double MembraneArg::getBondLength() const { return bondLength; }

double MembraneArg::getCutOffRadius() const { return cutOffRadius; }

int MembraneArg::getType() const { return type; }
