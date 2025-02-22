#pragma once

#include "Cell.h"
#include "model/LennardJonesModel.h"

#define SIXTH_RT 1.1224620483093729814335330496791795162324111106

/// Parameters for reflection boundary.
namespace ReflectiveBoundary {
// Parameters for reflection
constexpr double SIXTH_ROOT_OF_2 = SIXTH_RT;

// Initialize LennardJones model for reflective boundary.
static LennardJonesModel lennardJones{3e9};
}  // namespace ReflectiveBoundary

enum class BoundaryType {
  /// Boundary is an outflow boundary
  OUTFLOW,
  /// Boundary is a reflecting boundary
  REFLECT,
  /// Boundary is a periodic boundary
  PERIODIC [[maybe_unused]]
};

class LinkedCellsBoundary {
 private:
  /// The side of the linkedCellsContainer where the boundary lies on
  CubeSide side;

  /// The type of the boundary which defines how it behaves
  BoundaryType type;

  /// Reference to LinkedCellsContainer to set up structure of boundary

  /// Dimensions of cell grid of the corresponding LinkedCells container
  std::array<unsigned int, 3> cubeDimensions;

  /// A reference to the particles vector of the corresponding container.
  std::vector<Particle>* particlesVector;

  /// Left, lower, front corner of corresponding LinkedCells container in 3d
  std::array<double, 3>* leftLowerCorner;

  /// Right, upper, back corner of corresponding LinkedCells container in 3d
  std::array<double, 3>* rightUpperCorner;

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

  // E.g. access via: cellGridLocationTable[static_cast<size_t>(cubeSide::TOP)]
  std::array<unsigned int, 6> const cellGridLocationTable = {
      1,                      // Relevant cell location, cubeSide::LEFT
      cubeDimensions[0] - 2,  // Relevant cell location, cubeSide::RIGHT
      1,                      // Relevant cell location, cubeSide::TOP
      cubeDimensions[1] - 2,  // Relevant cell location, cubeSide::BOTTOM
      1,                      // Relevant cell location, cubeSide::FRONT
      cubeDimensions[2] - 2,  // Relevant cell location, cubeSide::BACK
  };

  // E.g. access via: haloLocationTable[static_cast<size_t>(cubeSide::TOP)]
  std::array<unsigned int, 6> const haloLocationTable = {
      0,                      // Relevant cell location, cubeSide::LEFT
      cubeDimensions[0] - 1,  // Relevant cell location, cubeSide::RIGHT
      0,                      // Relevant cell location, cubeSide::TOP
      cubeDimensions[1] - 1,  // Relevant cell location, cubeSide::BOTTOM
      0,                      // Relevant cell location, cubeSide::FRONT
      cubeDimensions[2] - 1,  // Relevant cell location, cubeSide::BACK
  };

  inline size_t getDimensionBySide(CubeSide s) const { return dimensionTable[static_cast<size_t>(s)]; }

  inline unsigned int getCellGridLocation(CubeSide s) const { return cellGridLocationTable[static_cast<size_t>(s)]; }

  inline unsigned int getHaloGridLocation(CubeSide s) const { return haloLocationTable[static_cast<size_t>(s)]; }

  /**
   * Takes particle and returns a location moved along the LinkedCells container cube backwards, to simulate a periodic
   * boundary. (It can be thought of being similar to a modulo operation applied to the location of the particle)
   * @param particle The particle for which we update the location
   * @return The new location
   */
  std::array<double, 3> getPeriodicLocation(Particle& particle);

 public:
  /// Pointers to connected boundary cells
  std::vector<cell*> connectedCells;

  /// Pointers to connected halo cells
  std::vector<cell*> connectedHalos;

  /**
   * Instantiate a LinkedCellsBoundary which is used to model the walls of a
   * LinkedCells Container.
   * @param side The side of the LinkedCells container which gets the boundary
   * @param type The type of the boundary (e.g. outflow, reflective, etc.)
   * @param cells Cells of the linkedCells container
   * @param particlesVector A pointer to the particlesVector of the container.
   *   This is required to allow e.g. particle deletion
   * @param dimensions The cell grid dimensions
   * @param leftLowerCorner The left lower front corner of the 3d container cube
   * @param rightUpperCorner The right upper back corner of the 3d container
   * cube
   */
  LinkedCellsBoundary(CubeSide side, BoundaryType type, std::vector<cell>& cells,
                      std::vector<Particle>* particlesVector, std::array<unsigned int, 3> dimensions,
                      std::array<double, 3>* leftLowerCorner, std::array<double, 3>* rightUpperCorner);

  /**
   * Returns distance of a particle to the boundary
   * @param particle Particle for which we measure the distance
   * @return The distance to the wall. The distance can be negative, because
   * a particle being left and right to a boundary is treated separately.
   */
  double getDistanceToBoundary(Particle const& particle) const;

  /**
   * Translate cell vector index to grid location
   * @param index the index that gets translated into a grid location
   * @return grid location {x, y, z} of cell
   */
  std::array<unsigned int, 3> getCellGridLocation(size_t index) const;

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
   * Implements the periodic boundary. Particles that leave the domain through a periodic boundary
   * get teleported to the other side of the LinkedCells container cube.
   */
  void teleportParticles();

  /**
   * Applies the effects of the current boundary
   */
  void apply();

  /**
   * @return Boundary cells on which the boundary operates on.
   */
  [[nodiscard]] std::vector<cell*> getConnectedCells() const { return connectedCells; }

  /**
   * @return Side on which the boundary lies. E.g. left side of LinkedCells
   * container cube.
   */
  [[maybe_unused]] CubeSide getSide() { return side; }

  /**
   * @return Type of the boundary. (Outflow, reflective, periodic)
   */
  BoundaryType getType() { return type; }
};
