/*
 * Particle.cpp
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#include "Particle.h"

#include <iostream>

#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"

Particle::Particle(int type_arg) {
  type = type_arg;
  spdlog::trace("Particle generated!");
  x = {0.0, 0.0, 0.0};
  v = {0.0, 0.0, 0.0};
  m = 1.0;
  f = {0., 0., 0.};
  old_f = {0., 0., 0.};
}

Particle::Particle(const Particle &other) {
  x = other.x;
  v = other.v;
  f = other.f;
  old_f = other.old_f;
  m = other.m;
  type = other.type;
  spdlog::trace("Particle generated by copy!");
}

Particle::Particle(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                   double m_arg, int type_arg)
    : x(x_arg), v(v_arg), m(m_arg), type(type_arg) {
  f = {0., 0., 0.};
  old_f = {0., 0., 0.};
  spdlog::trace("Particle generated!");
}

Particle::~Particle() { spdlog::trace("Particle destructed!"); }

double Particle::getM() const { return m; }

int Particle::getType() const { return type; }

std::string Particle::toString() const {
  std::stringstream stream;
  stream << "Particle: X:" << x << " v: " << v << " f: " << f
         << " old_f: " << old_f << " type: " << type;
  return stream.str();
}

bool Particle::operator==(Particle &other) {
  return (x == other.x) and (v == other.v) and (f == other.f) and
         (type == other.type) and (m == other.m) and (old_f == other.old_f);
}

std::ostream &operator<<(std::ostream &stream, Particle &p) {
  stream << p.toString();
  return stream;
}

void Particle::updateForces() {
  old_f = f;
  f = 0 * f;
}
