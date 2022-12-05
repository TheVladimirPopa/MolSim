#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include "LinkedCellsContainer.h"
#include "utils/ArrayUtils.h"
#include "model/LennardJonesModel.h"



// todo: do this properly
static constexpr double CUTOFFRADIUS{42}; // TODO: use correct value
static const LennardJonesModel lennardJones{CUTOFFRADIUS};
constexpr double SIGMA = 1.0;  // TODO: nicht hardcoden
constexpr double SIXTH_ROOT_OF_2 =
    1.1224620483093729814335330496791795162324111106139867534404095458;
const double viewDistance = SIXTH_ROOT_OF_2 * SIGMA;

class LinkedCellsBoundary {
 private:
  cubeSide side;
  LinkedCellsContainer& container;
  // std::array<int, 3>& cubeDimension;

  struct dimensionAndPosition {
    size_t dimensionIndex;
    unsigned int position;
  };

  std::unordered_map<cubeSide, std::pair<size_t, unsigned int>> const
      dimLocation = {
          // Map boundary side onto dimension and index in dimension
          {cubeSide::LEFT,   {0, 1}},
          {cubeSide::RIGHT,  {0, container.dimensions[0] - 2}},
          {cubeSide::TOP,    {1, 1}},
          {cubeSide::BOTTOM, {1, container.dimensions[1] - 2}},
          {cubeSide::FRONT,  {2, 1}},
          {cubeSide::BACK,   {2, container.dimensions[2] - 2}}
  };

 public:
  std::vector<LinkedCellsContainer::boundaryCell*> connectedCells;
  virtual void applyBoundaryLogic();

  /**
   * Instantiate a LinkedCellsBoundary which is used to model the walls of a
   * LinkedCells Container.
   * @param side The side of the LinkedCells container which gets the boundary
   * @param container The LinkedCells container which receives the boundary
   */
  LinkedCellsBoundary(cubeSide side, LinkedCellsContainer& container);

  /**
   * Given a particle within a boundary cell returns the distance of said
   * particle to the boundary.
   * @param particle The particle for which to return the distance
   * @return Distance to the boundary
   */
  double getDistanceToWall(Particle const& particle);

  /**
   * Deletes particles that have left the boundary and entered a halo cell.
   */
  void deleteOutFlow();


  /**
   * Reflects particles that are near the edge using the Lennard Jones force.
   * Reflection distance is per convention: (6th root of 2) * sigma.
   */
  void reflectParticles();
};
