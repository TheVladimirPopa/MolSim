#include "TestUtils.h"

VectorContainer TestUtils::makeContainer(unsigned size) {
  VectorContainer particle_container{};
  for (unsigned i = 0; i < size; i++) {
    particle_container.emplace_back(v3d{}, v3d{}, 1.0, 1);
  }

  return particle_container;
}

/**
 * Fills the container with particles and sets boundaries
 * @param container gets filled with particles
 */
void TestUtils::setupLinkedCellsContainer(LinkedCellsContainer &container) {
  ParticleGeneration::cuboid c1;
  c1.type = 1;
  c1.distance = 0.5;
  c1.position = {.1, .1, .1};
  c1.dimension = {9, 5, 4};
  c1.mass = 5000.;
  c1.velocity = {0., 0., .3};

  ParticleGeneration::addCuboidToParticleContainer(container, c1);

  container.setBoundaries({
      {CubeSide::LEFT, BoundaryType::PERIODIC},
      {CubeSide::RIGHT, BoundaryType::PERIODIC},
      {CubeSide::TOP, BoundaryType::REFLECT},
      {CubeSide::BOTTOM, BoundaryType::REFLECT},
      {CubeSide::FRONT, BoundaryType::PERIODIC},
      {CubeSide::BACK, BoundaryType::PERIODIC},
  });
}
/**
 * Applys one force calculation to all particles of a container
 * @param container the container on which forEachPair gets called
 */
void TestUtils::applyForceCalc(LinkedCellsContainer &container) {
  LennardJonesModel model{1.5};
  model.setDeltaT(1.);
  using P = Particle &;
  std::function<void(P, P)> addForces{
      [&model](P p1, P p2) { model.addForces(std::forward<P>(p1), std::forward<P>(p2)); }};
  container.forEachPair(addForces);
}

std::vector<Particle> TestUtils::getParticleVector(LinkedCellsContainer &container) {
  std::vector<Particle> particles;
  using P = Particle &;
  std::function<void(P)> saveParticles{[&particles](P p1) { particles.push_back(p1); }};

  container.forEach(saveParticles);
  return particles;
}