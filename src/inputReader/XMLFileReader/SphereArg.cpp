#include "SphereArg.h"
SphereArg::SphereArg(const std::array<double, 3>& position,
                     const std::array<double, 3>& velocity, double radius,
                     int dimension, double distance, double mass, int type)
    : position(position),
      velocity(velocity),
      radius(radius),
      dimension(dimension),
      distance(distance),
      mass(mass),
      type(type) {}

const std::array<double, 3>& SphereArg::getPosition() const { return position; }

const std::array<double, 3>& SphereArg::getVelocity() const { return velocity; }

double SphereArg::getRadius() const { return radius; }

int SphereArg::getDimension() const { return dimension; }

double SphereArg::getDistance() const { return distance; }

double SphereArg::getMass() const { return mass; }

int SphereArg::getType() const { return type; }
