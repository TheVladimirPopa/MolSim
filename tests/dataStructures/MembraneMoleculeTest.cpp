#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/MembraneMolecule.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include "inputReader/ParticleGenerator.h"
using MembraneInput = ParticleGeneration::membrane;
using v3d = std::array<double, 3>;

/**
 * @return A membrane struct that can be used to generate the actual molecule
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
TEST(MembraneMolecule, Essentials) {
  auto cont = makeSampleContainer();
  auto mem = makeSampleMembraneInput();
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  // Check location
  ASSERT_EQ(cont->getMolecules().size(), 1);
  MembraneMolecule& membrane = cont->getMolecules().back();
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
TEST(MembraneMolecule, ArtificialForces) {
  auto cont = makeSampleContainer();
  auto mem = makeSampleMembraneInput();
  mem.dimension = {5, 5, 1};
  mem.membraneForces = {
      ParticleGeneration::MembraneForce{0, 0, 0, 1, 0, 150}, ParticleGeneration::MembraneForce{1, 2, 0, 1, 0, 150},
      ParticleGeneration::MembraneForce{3, 1, 0, 1, 0, 155}, ParticleGeneration::MembraneForce{3, 2, 0, 1, 0, 155}};

  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  auto& molecule = cont->getMolecules().back();
  auto expectedForce = v3d{0, 0, 0};
  for (size_t i = 0; i < 157; i++) {
    if (i <= 155) {
      EXPECT_TRUE(molecule.hasArtificalForces()) << "Molecule lost artificial force too early.";
    } else {
      EXPECT_FALSE(molecule.hasArtificalForces()) << "Molecule kept artificial forces for too long";
    }

    if (i < 150) expectedForce[1] += 1;

    molecule.applyArtificialForces();
    EXPECT_EQ(cont->getParticlesRef()[0].getF(), expectedForce)
        << "Expected different force when applying artificial force";
  }
}

/**
 * Tests whether non neighbouring particles are correctly repulsed to avoid self penetration
 */
TEST(MembraneMolecule, RepulsiveForces) {
  auto mem = makeSampleMembraneInput();
  auto cont = makeSampleContainer();
  mem.dimension = {20, 1, 20};
  ParticleGeneration::addMembraneToParticleContainer(*cont, mem);

  auto& particle1 = cont->getParticlesRef()[0];
  auto& particle2 = cont->getParticlesRef()[10];

  auto x1 = v3d{10, 10, 10};
  particle1.setX(x1);

  auto x2 = v3d{11, 10, 10};
  particle2.setX(x2);

  cont->getMolecules().back().applyForce(particle1, particle2, 0, 10);

  auto expectedForce1 = v3d{-120., 0.0, 0.0};
  ASSERT_EQ(particle1.getF(), expectedForce1);
  ASSERT_EQ(particle2.getF(), (-1.0) * expectedForce1);
}