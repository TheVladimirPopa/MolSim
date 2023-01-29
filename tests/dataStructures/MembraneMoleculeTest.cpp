#include <algorithm>

#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/MembraneStructure.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include "inputReader/ParticleGenerator.h"
using MembraneInput = ParticleGeneration::membrane;
using SphereInput = ParticleGeneration::sphere;
using v3d = std::array<double, 3>;

/**
 * @return A membrane struct that can be used to generate the actual body
 */
MembraneInput makeSampleMembraneInput() {
  MembraneInput mem{};
  mem.bondLength = 2.2;
  mem.stiffness = 300.;
  mem.mass = 1.0;
  mem.dimension = std::array<int, 3>{2, 2, 1};
  mem.distance = 1.0;
  mem.position = {0, 0, 0};
  mem.velocity = {0, 0, 0};
  return mem;
}

/**
 * @return A membrane struct that can be used to generate the actual body
 */
SphereInput makeSampleSphereInput() {
  SphereInput sphere{};
  sphere.position = {30, 30, 30};
  sphere.radius = 3;
  sphere.dimension = 3;
  sphere.velocity = {0., 1., 2.};
  sphere.distance = {1.2};
  sphere.mass = {1.0};
  sphere.type = 1;
  return sphere;
}

/**
 * @return A basic linked cells container
 */
std::unique_ptr<LinkedCellsContainer> makeSampleContainer() {
  v3d ll{-5, -5, -5};
  v3d ur{55, 55, 55};
  return std::make_unique<LinkedCellsContainer>(10, ll, ur);
}

/**
 * Checks whether diagonal and non-diagonal forces are applied correctly within the membrane.
 */
TEST(MembraneStructure, Essentials) {
  auto cont = makeSampleContainer();
  auto mem = makeSampleMembraneInput();
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  // Check location
  ASSERT_EQ(cont->getStructureVectorRef().size(), 1);
  MembraneStructure& membrane = cont->getStructureVectorRef().back();
  EXPECT_EQ(membrane.size(), 4);

  std::vector<v3d> targetLocations{
      {0, 0, 0}, {0, mem.distance, 0}, {mem.distance, 0, 0}, {mem.distance, mem.distance, 0}};

  for (auto loc : targetLocations) {
    bool particleExistsAtLocation = false;

    for (auto& particle : membrane) {
      if (particle.getX() == loc) {
        particleExistsAtLocation = true;
        break;
      };
    }

    EXPECT_TRUE(particleExistsAtLocation);
  }

  // Check forces
  membrane.applyInternalForces();

  for (auto& p1 : membrane) {
    for (auto& p2 : membrane) {
      EXPECT_EQ(L2Norm(p1.getF()), L2Norm(p2.getF())) << "Forces where not applied equally to all particles.";
    }
  }

  for (auto& p : membrane) {
    for (double value : p.getF()) {
      value = std::round(value * 1000.0) / 1000.0;
      EXPECT_TRUE(value == 720.0 || value == -720.0 || value == 0) << "Force has wrong magnitude.";
    }
    EXPECT_GT(L2Norm(p.getF()), 0.0);
  }
}

/**
 * Checks whether artificial forces are applied correctly
 */
TEST(MembraneStructure, ArtificialForces) {
  auto cont = makeSampleContainer();
  auto mem = makeSampleMembraneInput();
  mem.dimension = {5, 5, 1};
  mem.membraneForces = {
      ParticleGeneration::MembraneForce{0, 0, 0, 1, 0, 150}, ParticleGeneration::MembraneForce{1, 2, 0, 1, 0, 150},
      ParticleGeneration::MembraneForce{3, 1, 0, 1, 0, 155}, ParticleGeneration::MembraneForce{3, 2, 0, 1, 0, 155}};

  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  auto& membrane = cont->getStructureVectorRef().back();
  auto expectedForce = v3d{0, 0, 0};
  for (size_t i = 0; i < 157; i++) {
    if (i <= 155) {
      EXPECT_TRUE(membrane.hasArtificalForces()) << "Membrane lost artificial force too early.";
    } else {
      EXPECT_FALSE(membrane.hasArtificalForces()) << "Membrane kept artificial forces for too long";
    }

    if (i < 150) expectedForce[1] += 1;

    membrane.applyArtificialForces();
    EXPECT_EQ(cont->getParticlesRef()[0].getF(), expectedForce)
        << "Expected different force when applying artificial force";
  }
}

/**
 * Tests whether non neighbouring particles are correctly repulsed to avoid self penetration
 */
TEST(MembraneStructure, RepulsiveForces) {
  auto mem = makeSampleMembraneInput();
  auto cont = makeSampleContainer();
  mem.dimension = {20, 1, 20};
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  auto& particle1 = cont->getParticlesRef()[0];
  auto& particle2 = cont->getParticlesRef()[10];
  ASSERT_EQ(particle1.getId(), 0) << "Membrane applied wrong id";
  ASSERT_EQ(particle2.getId(), 10) << "Membrane applied wrong id";

  auto x1 = v3d{10, 10, 10};
  particle1.setX(x1);

  auto x2 = v3d{11, 10, 10};
  particle2.setX(x2);

  cont->getStructureVectorRef().back().applyForce(particle1, particle2);

  auto expectedForce1 = v3d{-120., 0.0, 0.0};
  ASSERT_EQ(particle1.getF(), expectedForce1);
  ASSERT_EQ(particle2.getF(), (-1.0) * expectedForce1);
}

/**
 * Tests if repulse and attracting forces are applied based on particle distance.
 */
TEST(MembraneStructure, ForcesBig) {
  auto mem = makeSampleMembraneInput();
  auto cont = makeSampleContainer();

  mem.dimension = {6, 6, 1};
  mem.distance = 1.0;
  mem.bondLength = 1.0;

  ParticleGeneration::addSphereToParticleContainer(*cont, makeSampleSphereInput());
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);
  auto& membrane = cont->getStructureVectorRef().front();

  // Check whether all particles are actually repulsed when they are supposed to
  auto testParticlePair = [&](std::array<unsigned long, 3> pos1, std::array<unsigned long, 3> pos2) {
    auto& p1 = membrane.getParticleByPosition(pos1);
    auto& p2 = membrane.getParticleByPosition(pos2);

    if (&p1 == &p2) return;

    // Move particles to location
    auto orig1 = p1.getX();
    auto orig2 = p2.getX();

    p1.setX({15, 0, 0});
    p2.setX({17.5, 0, 0});

    // Apply forces
    p1.updateForces();
    p2.updateForces();
    membrane.applyForce(p1, p2);

    p1.setX(orig1);
    p2.setX(orig2);

    if (ArrayUtils::L2Norm(orig2 - orig1) < 1.5) {
      // Neighbours must attract
      ASSERT_GT(p1.getF()[0], 0) << "Neighbours should attract each other. Ids: " << p1.getId() << ", " << p2.getId();
    } else {
      // Non-neighbours must repel forces
      ASSERT_EQ(std::abs(p1.getF()[0]), 0) << "No force expected for >2^1/6 diff " << p1.getId() << ", " << p2.getId();
    }
  };

  // Check each possible combination of particles of the membrane interacting
  for (int i = 0; i < mem.dimension[0]; i++) {
    for (int j = 0; j < mem.dimension[1]; j++) {
      for (int k = 0; k < mem.dimension[0]; k++) {
        for (int l = 0; l < mem.dimension[1]; l++) {
          testParticlePair({static_cast<unsigned long>(i), static_cast<unsigned long>(j), 0},
                           {static_cast<unsigned long>(k), static_cast<unsigned long>(l), 0});
        }
      }
    }
  }
}

/**
 * Checks ids, iterators and applies one repulsive test force in the presence of other particles in the container.
 */
TEST(MembraneStructure, DisplacedIds) {
  auto mem = makeSampleMembraneInput();
  auto cont = makeSampleContainer();

  mem.dimension = {50, 50, 1};
  size_t memSize = mem.dimension[0] * mem.dimension[1] * mem.dimension[2];

  // Generate sphere and find out where it ends
  ParticleGeneration::addSphereToParticleContainer(*cont, makeSampleSphereInput());
  size_t sphereParticleIndex = cont->getParticlesRef().back().getId();

  // Add membrane and find sphere in the resized particle vector
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);
  auto& sphereParticle = cont->getParticlesRef()[sphereParticleIndex];

  // Test ids, iterators, etc.
  auto& membrane = cont->getStructureVectorRef().front();
  auto& firstParticle = *membrane.begin();
  auto& lastParticle = *(membrane.end() - 1);

  ASSERT_NE(&firstParticle, &lastParticle) << "Membrane structure is faulty";
  ASSERT_NE(&firstParticle, &sphereParticle) << "Membrane structure is faulty";
  ASSERT_NE(&lastParticle, &sphereParticle) << "Membrane structure is faulty";
  ASSERT_EQ(sphereParticle.getId() + 1, firstParticle.getId()) << "Membrane structure is faulty or wrong ids";
  ASSERT_EQ(sphereParticle.getId() + memSize, lastParticle.getId()) << "Membrane structure is faulty or wrong ids";
  ASSERT_EQ(membrane.size(), memSize) << "Membrane is miss-constructed";
  size_t size{0};
  for (auto begin = membrane.begin(); begin != membrane.end(); begin++) size++;
  ASSERT_EQ(size, membrane.size()) << "Membrane iterators are off";
}