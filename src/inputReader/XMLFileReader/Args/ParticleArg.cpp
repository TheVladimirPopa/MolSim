#include "ParticleArg.h"
ParticleArg::ParticleArg(int id, double epsilon, double sigma) : id(id), epsilon(epsilon), sigma(sigma) {}

int ParticleArg::getId() const { return id; }

double ParticleArg::getEpsilon() const { return epsilon; }

double ParticleArg::getSigma() const { return sigma; }
