#pragma once
#include <list>
using std::size_t;

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

  /// Returns whether the cell is empty
  inline bool isEmpty() { return particles.empty(); }
};