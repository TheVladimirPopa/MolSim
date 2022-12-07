#pragma once
#include <list>
#include <memory>
#include <stdexcept>

#include "IContainer.h"

/**
 * A cube placed on an XYZ grid. The keywords name the sides of the cube.
 *    +--------+
 *  /  TOP   / |
 * +--------+  | RIGHT
 * |        |  +
 * | FRONT  | /
 * +--------+
 * LEFT and RIGHT are relative to the dimension[0] axis.
 * TOP and BOTTOM are relative to the dimension[1] axis.
 * FRONT and BACK are relative to the dimension[2] axis.
 */
enum class cubeSide {
  LEFT = 0,    // dimension[0]
  RIGHT = 1,   // dimension[0]
  TOP = 2,     // dimension[1]
  BOTTOM = 3,  // dimension[1]
  FRONT = 4,   // dimension[2]
  BACK = 5     // dimension[2]
};

class LinkedCellsBoundary;
enum class cellType {
  /// A cell that is inside the domain bounding box but doesn't touch the
  /// border
  inner,
  /// A cell that is inside the domain bounding box but touches the border
  boundary,
  /// A cell that lies outside the domain bounding box
  halo
};

/**
   * A struct representing a single cell with its type and pointer to the
   * particles in it
 */
struct cell {
  /// List of pointers to the particles which are currently inside the cell
  std::list<size_t> particles{};
  /// The type of the cell
  cellType type = cellType::inner;
  /// Index of cell in the LinkedCellsContainer cell vector
  size_t cellVectorIndex;

  explicit cell(cellType t, size_t index)
      : particles{}, type{t}, cellVectorIndex{index} {}

};

class LinkedCellsContainer : public IContainer {
 private:

  /// The boundaries the container has
  std::vector<LinkedCellsBoundary>* boundaries{};

  /// The vector containing all the particles
  std::vector<Particle> particlesVector;

  /// A vector containing all the cells
  std::vector<cell> cells;

  /// Edge length of a cell
  double gridSize;

  /// The left lower corner of the domain bounding box
  std::array<double, 3> leftLowerCorner;

  /// The right upper corner of the domain bounding box
  std::array<double, 3> rightUpperCorner;

  /// The count of cells in each direction, halo cells are included
  std::array<unsigned int, 3> dimensions{};

  /**
   * Compute the index of the cell containing the given position
   * @param position A 3d array containing the position
   * @return The index in the cells vector that has the cell in which the
   * position is located
   */
  size_t getCellIndexOfPosition(std::array<double, 3> &position);

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

  /**
   * Translate cell vector index to grid location
   * @param index the index that gets translated into a grid location
   * @return grid location {x, y, z} of cell
   */
  std::array<unsigned int, 3> getCoordFromVectorIndex(size_t index);

  /// All the offsets for adjacent cells which have a greater index than the
  /// current one
  std::array<size_t, 14> indexOffsetAdjacent{};

  // The boundaries need access to dimensions, particles, cells
  friend class LinkedCellsBoundary;

 public:
  /**
   * Instantiating a LinkedCells Container
   * @param cellSize The edge length of a cell
   * @param leftLowerBound The left lower corner of the domain bounding box
   * @param rightUpperBound The right upper corner of the domain bounding box
   * @note This method assumes the boundingBox dimensions are an multiple of the
   * cell size in each dimension
   */
  LinkedCellsContainer(double cellSize, std::array<double, 3> &leftLowerBound,
                       std::array<double, 3> &rightUpperBound);

  ~LinkedCellsContainer() override = default;

  void forEach(std::function<void(Particle &)> &unaryFunction) override;

  void forEachPair(
      std::function<void(Particle &, Particle &)> &binaryFunction) override;

  void reserve(size_t amount) override;

  size_t capacity() override;

  size_t size() override;

  void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg,
                    double m_arg, int type) override;

  /**
   * Reorders the datastructure to make sure all particles are in the correct
   * cell
   */
  void recalculateStructure();

  /**
   * Method returning the cells vector. ONLY USED FOR TESTING
   * @return a const reference to the cells vector
   */
  const std::vector<cell> &getCellsVector() { return cells; }

  /**
   * Returns the boundaries of the LinkedCells container
   * @return boundaries
   */
  std::vector<LinkedCellsBoundary>* getBoundaries() { return boundaries; }

  /**
   * @return Cell dimensions in number of cells per dimension
   */
  std::array<unsigned int, 3> getDimensions() { return dimensions; }
};
