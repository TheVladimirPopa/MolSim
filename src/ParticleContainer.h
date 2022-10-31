#pragma once

#include <algorithm>
#include <vector>

#include "Particle.h"
#include "utils/Interfaces.h"

class IModel;
class ParticleContainer {
 private:
  std::vector<Particle> particles;
  IModel* model = nullptr;

 public:
  ParticleContainer() {}

  ParticleContainer(IModel* model) : model(model) {}

  ParticleContainer(std::vector<Particle> particles, IModel* model)
      : particles(particles), model(model) {}

  void addParticle(Particle p) { particles.push_back(std::move(p)); }
  auto begin() { return particles.begin(); }
  auto end() { return particles.end(); }
  auto size() { return particles.size(); }
  bool operator==(ParticleContainer const& other) const {
    return false; // todo
  }
  auto operator[] (unsigned long n) const { return particles[n]; }

  void setModel(IModel* m) {
    model = m;
  }


};

