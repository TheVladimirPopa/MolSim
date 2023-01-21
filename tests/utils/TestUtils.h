#pragma once
#include "dataStructures/LinkedCellsContainer.h"
#include "dataStructures/VectorContainer.h"
#include "inputReader/ParticleGenerator.h"
using v3d = std::array<double, 3>;

namespace TestUtils {

/**
 * Generates container for the ParticleContainer test suite.
 */
VectorContainer makeContainer(unsigned size);

/**
 * Fills the container with particles and sets boundaries
 * @param container gets filled with particles
 */
void setupLinkedCellsContainer(LinkedCellsContainer &container);

/**
 * Applys one force calculation to all particles of a container
 * @param container the container on which forEachPair gets called
 */
void applyForceCalc(LinkedCellsContainer &container);

std::vector<Particle> getParticleVector(LinkedCellsContainer &container);

}  // end namespace TestUtils
