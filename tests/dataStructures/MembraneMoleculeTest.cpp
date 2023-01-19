#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/MembraneMolecule.h"
#include "gtest/gtest.h"
#include "utils/ArrayUtils.h"
using ArrayUtils::L2Norm;
#include "inputReader/ParticleGenerator.h"
using MembraneInput = ParticleGeneration::membrane;


TEST(MembraneMolecule, Essentials) {
  MembraneInput mem{};
  mem.bondLength = 2.2;
  mem.stiffness = 300.;
  mem.mass = 1.0;
  mem.dimension = std::array<int, 3>{2, 2, 1};
  mem.distance = 1.0;
  mem.position = {0, 0, 0};
  mem.velocity = {0, 0, 0};

  std::array<double, 3> ll{-5, -5, -5};
  std::array<double, 3> ur{25, 25, 25};

  LinkedCellsContainer cont{10, ll, ur};

  ParticleGeneration::addMembraneToParticleContainer(cont, mem);

  // Check location
  ASSERT_EQ(cont.getMolecules().size(), 1);
  MembraneMolecule& membrane = cont.getMolecules().back();
  EXPECT_EQ(membrane.size(), 4);

  std::vector<std::array<double, 3>> targetLocations{
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