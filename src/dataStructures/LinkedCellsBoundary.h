#pragma once

#include "LinkedCellsContainer.h"
#include "model/LennardJonesModel.h"

#define QUASI_INFINITE 1e300
#define SIGMA_PLACEHOLDER 1.0
#define SIXTH_RT 1.1224620483093729814335330496791795162324111106139867534404095

/// Parameters for reflection boundary.
namespace ReflectiveBoundary {
// Initialize LennardJones model for reflective boundary.
static constexpr double CUTOFF_RADIUS{QUASI_INFINITE};
static const LennardJonesModel lennardJones{CUTOFF_RADIUS};

// Parameters for reflection
constexpr double SIGMA = SIGMA_PLACEHOLDER;
constexpr double SIXTH_ROOT_OF_2 = SIXTH_RT;
constexpr double reflectDistance = SIXTH_ROOT_OF_2 * SIGMA;
}  // namespace ReflectiveBoundary

enum class boundaryType {
  /// Boundary is an outflow boundary
  OUTFLOW,
  /// Boundary is a reflecing boundary
  REFLECT,
  /// Boundary is a periodic boundary
  PERIODIC
};

class LinkedCellsBoundary {
 private:
  /// The side of the linkedCellsContainer where the boundary lies on
  cubeSide side;

  /// The type of the boundary which defines how it behaves
  boundaryType type;

  /// Reference to LinkedCellsContainer to set up structure of boundary
  LinkedCellsContainer& container;

  // Contains characteristic position parameters for boundary cells based on
  // cube side. This is an array instead of a map for fast lookup times.
  // E.g. access via: dimensionTable[static_cast<size_t>(cubeSide::TOP)]
  std::array<size_t, 6> const dimensionTable = {
      0,  // Relevant dimension for cubeSide::LEFT
      0,  // Relevant dimension for cubeSide::RIGHT
      1,  // Relevant dimension for cubeSide::TOP
      1,  // Relevant dimension for cubeSide::BOTTOM
      2,  // Relevant dimension for cubeSide::FRONT
      2   // Relevant dimension for cubeSide::BACK
  };

  // E.g. access via: cellSlotTable[static_cast<size_t>(cubeSide::TOP)]
  std::array<unsigned int, 6> const cellGridLocationTable = {
      1,                            // Relevant cell location, cubeSide::LEFT
      container.dimensions[0] - 2,  // Relevant cell location, cubeSide::RIGHT
      1,                            // Relevant cell location, cubeSide::TOP
      container.dimensions[1] - 2,  // Relevant cell location, cubeSide::BOTTOM
      1,                            // Relevant cell location, cubeSide::FRONT
      container.dimensions[2] - 2,  // Relevant cell location, cubeSide::BACK
  };

  inline size_t getDimensionBySide(cubeSide side) {
    return dimensionTable[static_cast<size_t>(side)];
  }

  inline unsigned int getCellGridLocation(cubeSide side) {
    return cellGridLocationTable[static_cast<size_t>(side)];
  }

 public:
  std::vector<LinkedCellsContainer::boundaryCell*> connectedCells;

  /**
   * Instantiate a LinkedCellsBoundary which is used to model the walls of a
   * LinkedCells Container.
   * @param side The side of the LinkedCells container which gets the boundary
   * @param type The type of the boundary (e.g. outflow, reflective, etc.)
   * @param container The LinkedCells container which receives the boundary
   */
  LinkedCellsBoundary(cubeSide side, boundaryType type,
                      LinkedCellsContainer& container);

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

  /**
   * Generate boundaries based on side and type and adds them to the container
   * @param sideAndType Pairs of the side and corresponding boundary type
   * @param container The LinkedCellsContainer that receives the boundaries
   * @return The generated boundaries
   */
  static void setBoundaries(
      std::vector<std::pair<cubeSide, boundaryType>> sideAndType,
      LinkedCellsContainer container);
};
