#include "CuboidArg.h"
CuboidArg::CuboidArg(const std::array<double, 3>& position, const std::array<double, 3>& velocity,
                     const std::array<int, 3>& dimension, double distance, double mass, int type)
    : position(position), velocity(velocity), dimension(dimension), distance(distance), mass(mass), type(type) {}

const std::array<double, 3>& CuboidArg::getPosition() const { return position; }

const std::array<double, 3>& CuboidArg::getVelocity() const { return velocity; }

const std::array<int, 3>& CuboidArg::getDimension() const { return dimension; }

double CuboidArg::getDistance() const { return distance; }

double CuboidArg::getMass() const { return mass; }

int CuboidArg::getType() const { return type; }
