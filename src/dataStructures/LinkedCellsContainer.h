#pragma once
#include <list>

#include "IContainer.h"
class LinkedCellsContainer : public IContainer {
 private:
  enum cellType {
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
    cellType type = inner;

    explicit cell(cellType t) : particles{}, type{t} {}
  };

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
  size_t getVectorIndexFromCord(size_t x, size_t y, size_t z);

  /** All the offsets for adjacent cells which have a greater index than the
   * current one, it has a length of 14 since their are 26 adjacent cells to one
   * cell and only 13 of them have a greater index. Offset of 0 is also added
   * because particles obviously affect each other within one cell
   */
  std::array<size_t, 14> indexOffsetAdjacent{};

  /**
   * Reorders the datastructure to make sure all particles are in the correct
   * cell, is automatically called before each forEachPair call
   */
  void recalculateStructure();

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
   * Method returning the cells vector. ONLY USED FOR TESTING
   * @return a const reference to the cells vector
   */
  const std::vector<cell> &getCellsVector() { return cells; }
};
