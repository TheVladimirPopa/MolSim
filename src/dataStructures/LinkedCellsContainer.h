#pragma once
#include <fstream>
#include <list>
#include <memory>
#include <stdexcept>

#include "Cell.h"
#include "IContainer.h"
#include "LinkedCellsBoundary.h"

/**  \image html runtime-linkedcells.png width=900px
 *    \image latex runtime-linkedcells.png "Runtime" width = [goodwidth]cm
 */
class LinkedCellsContainer : public IContainer {
 protected:
  /// The vector containing all the particles
  std::vector<Particle> particlesVector;

  /// The vector containing all structures. (Only membranes for now, worksheet 5)
  std::vector<MembraneStructure> structuresVector;

  /// A vector containing all the cells
  std::vector<cell> cells;

  /// The boundaries the container has
  std::vector<LinkedCellsBoundary> boundaries{};

  /// Whether the container uses periodic boundaries
  bool hasPeriodicBoundaries;

  /// Edge length of a cell
  double gridSize;

  /// The left lower corner of the domain bounding box
  std::array<double, 3> leftLowerCorner;

  /// The right upper corner of the domain bounding box
  std::array<double, 3> rightUpperCorner;

  /// The count of cells in each direction, halo cells are included
  std::array<unsigned int, 3> dimensions{};

  /// The geometrical size of the LinkedCells container cube
  std::array<double, 3> cubeSize;

  /**
   * Compute the index of the cell containing the given position
   * @param position A 3d array containing the position
   * @return The index in the cells vector that has the cell in which the
   * position is located
   */
  size_t getCellIndexOfPosition(std::array<double, 3> const &position);

  /**
   * Get the index from a 3d cell coordinate
   * @param x How many cell it is in the x direction
   * @param y How many cell it is in the y direction
   * @param z How many cell it is in the z direction
   * @return The index in the cells vector
   * @note Starting with the halo cell, so left lower corner oft the domain
   * bounding box has coordinates (1,1,1)
   */
  size_t getVectorIndexFromCoord(size_t x, size_t y, size_t z);

 public:
  /** All the offsets for adjacent cells which have a greater index than the
   * current one, it has a length of 14 since their are 26 adjacent cells to one
   * cell and only 13 of them have a greater index. Offset of 0 is also added
   * because particles obviously affect each other within one cell
   */
  std::array<size_t, 14> indexOffsetAdjacent{};

 protected:
  /**
   * Links up boundary cells and halo cells depending on the configuration of periodic boundaries that was applied.
   */
  void linkBoundaryToHaloCells();

  /**
   * Links all corner boundary cells to their halo cells on the opposite side on the cube.
   */
  void linkCornerBoundaryCells();

  // The boundaries need access to dimensions, particles, cells
  friend class LinkedCellsBoundary;

 public:
  /**
   * Instantiating a LinkedCells Container
   * @param cellSize The edge length of a cell
   * @param leftLowerBound The left lower corner of the domain bounding box
   * @param rightUpperBound The right upper corner of the domain bounding box
   */
  LinkedCellsContainer(double cellSize, std::array<double, 3> leftLowerBound, std::array<double, 3> rightUpperBound);

  ~LinkedCellsContainer() override = default;

  void forEach(std::function<void(Particle &)> &unaryFunction) override;

  void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) override;

  /**
   * Applies binaryFunction to all particles within the same cell and surrounding cell of the particle.
   * Use these function for external, unlinked particles. I. e. for force calculation with ghost particles!
   * @param particle Particle for which the cell is determined and which will be used to build pairs
   * @param binaryFunction The function which will be applied to the pairs: binaryFunction(particle, other_particles)
   * @note If the input particle A is linked in one of the cells, the function will call binaryFunction(A,A)
   */
  void forEachNeighbor(Particle &particle, std::function<void(Particle &, Particle &)> &binaryFunction);

  void reserve(size_t amount) override;

  size_t capacity() override;

  size_t size() override;

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg, int type) override;

  void push_back(Particle &particle) override;

  void push_back(MembraneStructure membrane);

  /**
   * Reorders the datastructure to make sure all particles are in the correct
   * cell, is automatically called before each forEachPair call
   */
  void recalculateStructure();

  /**
   * Sets boundaries of the chosen type on the chosen side of the container
   * @param sideAndType Pairs of the side and corresponding boundary type
   * @note This does not check whether the boundaries are sensible. Do not
   * apply multiple boundaries on the same side of the container, unless this is
   * exactly what you want.
   */
  [[maybe_unused]] void setBoundaries(std::vector<std::pair<CubeSide, BoundaryType>> sideAndType);

  /**
   * Applies the effects of all boundaries on the container
   */
  inline void applyBoundaries() {
    for (auto &boundary : boundaries) boundary.apply();
  }

  /**
   * Applies function to particle pairs through periodic boundaries.
   * @param binaryFunction The function that gets applied onto each particle pair
   */
  void applyPeriodicForces(std::function<void(Particle &, Particle &)> &binaryFunction);

  /**
   * Method returning the cells vector. ONLY USED FOR TESTING
   * @return a const reference to the cells vector
   */
  const std::vector<cell> &getCellsVector() { return cells; }

  /**
   * Returns the boundaries of the LinkedCells container
   * @return boundaries
   */
  std::vector<LinkedCellsBoundary> &getBoundaries() { return boundaries; }
  /**
   * @return Cell dimensions in number of cells per dimension
   */
  std::array<unsigned int, 3> getDimensions() { return dimensions; }

  /**
   * Returns the size of the grid of the LinkedCells container
   * @return the grid size
   */
  [[nodiscard]] double getGridSize() const { return gridSize; }
  /**
   * Returns the left lower corner bound of the LinkedCells container
   * @return the leftLowerCorner
   */
  [[nodiscard]] std::array<double, 3> getLeftLowerCorner() const { return leftLowerCorner; }
  /**
   * Returns the right upper corner bound of the LinkedCells container
   * @return the rightUpperCorner
   */
  [[nodiscard]] std::array<double, 3> getRightUpperCorner() const { return rightUpperCorner; }

  /**
   * Returns structures that are attached to the container
   * @return attached structures
   */
  [[nodiscard]] std::vector<MembraneStructure> &getStructureVectorRef() { return structuresVector; };

  /**
   * Returns reference to particles vector
   * @return Reference to particles vector
   */
  [[nodiscard]] std::vector<Particle> &getParticlesRef() override { return particlesVector; }

  /**
   * @return True if the linked cells container has structures
   */
  [[nodiscard]] bool containsStructures() { return !structuresVector.empty(); };
};
