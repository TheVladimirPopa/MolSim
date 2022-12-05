#pragma once
#include <list>

#include "IContainer.h"

enum class cubeSide {
  LEFT = 0,    // dimension[0]
  RIGHT = 1,   // dimension[0]
  TOP = 2,     // dimension[1]
  BOTTOM = 3,  // dimension[1]
  FRONT = 4,   // dimension[2]
  BACK = 5     // dimension[2]
};


class LinkedCellsBoundary;

class LinkedCellsContainer : public IContainer {
 private:
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

  // todo: documentation
  struct boundaryCell : cell {
    enum class boundaryCellType {
      // Determines the number of neighbouring halo cells a boundary cell has
      normal,  // cell has 1 neighboring halo cell
      edge,    // cell has 3 neighboring halo cells
      corner   // cell has 7 neighboring halo cells
    };
    enum class boundarySideType { left, right, up, down, front, back };
    boundaryCellType cornerType;
    boundarySideType boundarySide; // todo: 1. besser vector, 2. lohnt sich das?
    std::vector<size_t> neighborHaloIndices; // todo: besser pointer?

    explicit boundaryCell(size_t index,
                          std::array<unsigned int, 3> &dim,
                          std::vector<cubeSide> boundaries)
        : cell{cellType::boundary, index} {
      // Todo: determine type based upon index and dimension
      // auto location3d = getCoordFromVectorIndexStatic(index, dim);

      // Determine corner type (which determines number of neighbor cells)
      if (boundaries.size() <= 1) cornerType = boundaryCellType::normal;
      if (boundaries.size() == 2) cornerType = boundaryCellType::edge;
      if (boundaries.size() >= 3) cornerType = boundaryCellType::corner;

      // Store indices of neighbor halo cells
      for (auto b : boundaries)
        neighborHaloIndices.push_back(
            getIndexByCubeSide(cellVectorIndex, b, dim));

      // Helper that calculates die index of a diagonal cell
      auto addDiagonalIndices = [this, &dim](size_t indexDiagonal,
                                             cubeSide sideA, cubeSide sideB) {
        // E.g. boundaries = { top, right }, we want the top right field
        // first get index of top field
        indexDiagonal = getIndexByCubeSide(indexDiagonal, sideA, dim);

        // Then index of right field
        indexDiagonal = getIndexByCubeSide(indexDiagonal, sideB, dim);
        neighborHaloIndices.push_back(indexDiagonal);
      };

      if (cornerType == boundaryCellType::edge) {
        // boundaries is size 2, or there is a bug
        addDiagonalIndices(cellVectorIndex, boundaries[0], boundaries[1]);
      }

      if (cornerType == boundaryCellType::corner) {
        // boundaries is size 3, or there is a bug
        addDiagonalIndices(cellVectorIndex, boundaries[0], boundaries[1]);
        addDiagonalIndices(cellVectorIndex, boundaries[0], boundaries[2]);
        addDiagonalIndices(cellVectorIndex, boundaries[1], boundaries[2]);

        // E.g. we want the cell that is top, right, front of our boundary cell:
        // take index and update it with directions { top, right, front }
        size_t i = cellVectorIndex;
        for (auto b : boundaries) i = getIndexByCubeSide(i, b, dim);
        neighborHaloIndices.push_back(i);
      }
    }

    static unsigned int getIndexByCubeSide(size_t index, cubeSide side,
                                           std::array<unsigned int, 3> &dim) {
      if (side == cubeSide::LEFT) return index - 1;
      if (side == cubeSide::RIGHT) return index + 1;
      if (side == cubeSide::TOP) return index - dim[0];
      if (side == cubeSide::BOTTOM) return index + dim[0];
      if (side == cubeSide::FRONT) return index - dim[0] * dim[1];
      if (side == cubeSide::BACK) return index + dim[0] * dim[1];

      return -1;
    }
  };

  /* TODO
  struct boundaries {
    const size_t numberOfSides;
    ILinkedCellsBoundary left{};
    ILinkedCellsBoundary right{};
    ILinkedCellsBoundary top{};
    ILinkedCellsBoundary bottom{};
    ILinkedCellsBoundary front{};
    ILinkedCellsBoundary back{};

    boundaries(ILinkedCellsBoundary left, ILinkedCellsBoundary right,
               ILinkedCellsBoundary top, ILinkedCellsBoundary bottom)
        : left{left}, right{right}, top{top}, bottom{bottom}, numberOfSides{4} {}

    boundaries(ILinkedCellsBoundary left, ILinkedCellsBoundary right,
               ILinkedCellsBoundary top, ILinkedCellsBoundary bottom,
               ILinkedCellsBoundary front, ILinkedCellsBoundary back)
        : left{left},
          right{right},
          top{top},
          bottom{bottom},
          back{back},
          front({front}),
          numberOfSides{6} {}
  };

  boundaries boundary; */

  /// The vector containing all the particles
  std::vector<Particle> particlesVector;

  /// A vector containing all the cells
  std::vector<cell> cells;

  /// The vector
  std::vector<boundaryCell *> boundaryCells;

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

  /// All the offsets for adjacent cells which have a greater index than the
  /// current one
  std::array<size_t, 14> indexOffsetAdjacent{};

  /**
   * Reorders the datastructure to make sure all particles are in the correct
   * cell
   */
  void recalculateStructure();

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
   * Method returning the cells vector. ONLY USED FOR TESTING
   * @return a const reference to the cells vector
   */
  const std::vector<cell> &getCellsVector() { return cells; }

  // Todo: doc
  std::array<unsigned int, 3> getCoordFromVectorIndex(size_t index);

  std::array<unsigned int, 3> getDimensions() { return dimensions; };
  static std::array<unsigned int, 3> getCoordFromVectorIndexStatic(
      unsigned int index, std::array<unsigned int, 3> &dim);
};
