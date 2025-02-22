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
  m = 0.0;
  f = {0., 0., 0.};
  old_f = {0., 0., 0.};
  m = 1.0;
  epsilon = getLJEpsilon(type_arg);
  sigma = getLJSigma(type_arg);
  structureId = NOT_IN_STRUCTURE;
  particleId = NO_UNIQUE_ID;
  isDeleted_ = false;
  periodicBoundariesCrossed = {0, 0, 0};
  lastPosition = {0., 0., 0.};
}

Particle::Particle(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type_arg)
    : x(x_arg),
      v(v_arg),
      f{0., 0., 0.},
      old_f{0., 0., 0.},
      m(m_arg),
      type(type_arg),
      epsilon{getLJEpsilon(type_arg)},
      sigma{getLJSigma(type_arg)},
      periodicBoundariesCrossed{0, 0, 0},
      isDeleted_{false},
      structureId{NOT_IN_STRUCTURE},
      particleId{NO_UNIQUE_ID},
      lastPosition{0., 0., 0.} {
  spdlog::trace("Particle generated!");
}

Particle::~Particle() { spdlog::trace("Particle destructed!"); }

double Particle::getM() const { return m; }

int Particle::getType() const { return type; }

std::string Particle::toString() const {
  std::stringstream stream;
  stream << "Particle: X:" << x << " v: " << v << " f: " << f << " old_f: " << old_f << " type: " << type;
  return stream.str();
}

bool Particle::operator==(Particle &other) {
  return (x == other.x) and (v == other.v) and (f == other.f) and (type == other.type) and (m == other.m) and
         (old_f == other.old_f);
}

std::ostream &operator<<(std::ostream &stream, Particle &p) {
  stream << p.toString();
  return stream;
}

void Particle::updateForces() {
  old_f = f;
  f = 0 * f;
}

void Particle::registerParticleType(int type, double epsilon, double sigma) {
  if (typeToParametersMap.find(type) != typeToParametersMap.end()) {
    spdlog::warn("ParticleType: {} has already been registered. Ignoring additional addition.", type);
    return;
  }

  typeToParametersMap.insert({type, ParticleType{epsilon, sigma}});
}

Particle::ParticleType Particle::getLJParticleType(int type) {
  constexpr ParticleType DEFAULT{5.0, 1.0};

  auto searchResult = typeToParametersMap.find(type);
  return (searchResult == typeToParametersMap.end()) ? DEFAULT : searchResult->second;
}

void Particle::applySymmetricForce(Particle &partner, const std::array<double, 3> &force) {
  f = f + force;
  partner.f = partner.f - force;
}

void Particle::applyForce(const std::array<double, 3> &force) { f = f + force; }