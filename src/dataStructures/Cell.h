#pragma once
#include <algorithm>
#include <array>
#include <list>
#include <vector>
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
  BACK = 5,    // dimension[2]
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

  /// Dimensions of the grid the cell is part of. This is used to calculate e.g. the position of periodic halo neighbor
  std::array<unsigned int, 3> gridDimensions;  // TODO: REFERENCE

  /// A reference to the 3D size of the LinkedCells container
  std::array<double, 3> cubeSize;

  /// A struct describing a link partner and the offset that must be applied for particles to move into the linkpartner
  struct periodicPartner {
    cell* pCell;
    std::array<double, 3> offset;
  };

  /// Periodic partners of cell. Halo cells will take the particles of their partner and simulate their forces.
  std::vector<periodicPartner> periodicPartners;

  explicit cell(cellType t, size_t index, std::array<unsigned int, 3>& dimensions, std::array<double, 3>& cubeSize)
      : particles{}, type{t}, cellVectorIndex{index}, gridDimensions{dimensions}, cubeSize{cubeSize} {}

  /// Returns whether the cell is empty
  inline bool isEmpty() { return particles.empty(); }

  // General Helper functions: -------

  /**
   * Translate cell vector index to grid location
   * @param index the index that gets translated into a grid location
   * @return grid location {x, y, z} of cell
   */
  std::array<unsigned int, 3> getCellGridLocation() const {
    unsigned int index(cellVectorIndex);
    unsigned int cellsPerLayer = gridDimensions[0] * gridDimensions[1];
    unsigned int layerCount = index / cellsPerLayer;
    index -= layerCount * cellsPerLayer;

    unsigned int lineCount = index / gridDimensions[0];
    unsigned int cellCount = index - (lineCount * gridDimensions[0]);

    return {cellCount, lineCount, layerCount};
  }

  /**
   * Given the position of a boundary cell and a side, will find the position of the halo cell that shares coordinates
   * with the given position with the exception of one component. E.g. Input a left boundary cell with side right
   * to get the halo cell that is parallel to the boundary cell on the opposite side of the LinkedCells container.
   * @param pos Position for which the position of a halo cell is determined
   * @param side Side of the container where the halo cell lies
   * @return Position of halo cell
   */
  std::array<unsigned int, 3> getHaloPos(std::array<unsigned int, 3> pos, cubeSide side) {
    size_t dimIndex = static_cast<size_t>(side) / 2;
    bool isLeftRelative = (side == cubeSide::LEFT || side == cubeSide::TOP || side == cubeSide::FRONT);
    pos[dimIndex] = isLeftRelative ? 0 : gridDimensions[dimIndex] - 1;
    return pos;
  }

  /**
   * Calculates distances between two planes of the LinkedCells container. Adds that distance / offset onto the input
   * offset.
   * @param plane Plane for which the distance to the other, parallel plane should be determined. E.g. Get distance of
   * LEFT plane to RIGHT plane
   * @param offset Offset onto which the determined distance will get added onto
   * @return New offset
   */
  std::array<double, 3> getPlaneDistance(cubeSide plane, std::array<double, 3> offset) {
    size_t dimIndex = static_cast<size_t>(plane) / 2;
    offset[dimIndex] = cubeSize[dimIndex];
    if (static_cast<size_t>(plane) % 2 == 1) offset[dimIndex] *= (-1.0);
    return offset;
  }

  /**
   * Maps a cubeSide onto it's opposite partner. E.g. LEFT onto RIGHT
   * @param side Side for which we find the opposite
   * @return The opposite side
   */
  static cubeSide getOppositeSide(cubeSide side) {
    // How sides on cube match up
    const std::vector<std::pair<cubeSide, cubeSide>> matchingSides = {
        {cubeSide::LEFT, cubeSide::RIGHT}, {cubeSide::RIGHT, cubeSide::LEFT}, {cubeSide::TOP, cubeSide::BOTTOM},
        {cubeSide::BOTTOM, cubeSide::TOP}, {cubeSide::FRONT, cubeSide::BACK}, {cubeSide::BACK, cubeSide::FRONT}};

    // Map LEFT -> RIGHT, RIGHT -> LEFT, TOP -> BOTTOM, ...
    return std::find_if(matchingSides.begin(), matchingSides.end(), [&side](auto pair) { return pair.first == side; })
        ->second;
  }

  // Periodic cells helper functions: -------

  /**
   * Links a boundary cell and a halo cell uniquely as link partners.
   * @param pos Position in cell grid of the cell that gets linked to the current cell
   * @param cells The cells vector which will is used to set pointers in both cells
   * @param offset What will be stored as geometrical offset for the partner. This is used to later teleport particles,
   * when applying forces through a periodic boundary.
   */
  void linkPartnerUnique(std::array<unsigned int, 3> pos, std::vector<cell>& cells, std::array<double, 3> offset) {
    size_t ghostIndex = pos[0] + (pos[1] * gridDimensions[0]) + (pos[2] * gridDimensions[0] * gridDimensions[1]);

    cell* ghost = &cells[ghostIndex];
    // push halo once into boundary cell
    auto isLinkPartner = [&ghost](periodicPartner p) { return p.pCell == ghost; };
    if (std::find_if(periodicPartners.begin(), periodicPartners.end(), isLinkPartner) == periodicPartners.end()) {
      periodicPartners.push_back({ghost, offset});

      // push boundary cell once into halo cell with negated offset
      for (int i = 0; i < 3; i++) offset[i] = (offset[i] == 0.0) ? 0.0 : -offset[i];

      (*ghost).periodicPartners.push_back({this, offset});
    }
  }

  /**
   * Links cell to halo cell, where the halo cell given by the cubeSide is taken.
   * @param side Of the halo cell that gets linked to the current cell. E. g. for LEFT cell, take RIGHT halo, to
   * simulate forces reaching through periodic boundary.
   * @param cells Cells vector to link both cells correctly.
   */
  void linkHaloSidePartner(cubeSide side, std::vector<cell>& cells) {
    auto ghostPos = getCellGridLocation();
    ghostPos = getHaloPos(ghostPos, side);

    std::array<double, 3> offset{};
    offset = getPlaneDistance(side, offset);

    linkPartnerUnique(ghostPos, cells, offset);
  }

  /**
   * Links cell to halo cell, where the halo cell given by the two cubeSides is taken. This only makes sense for
   * corner cells. E.g. if this function is used on a left, lower boundary cell and side1 = RIGHT, side2 = TOP, the
   * boundary cell will be link with the diagonal halo cell, that is found by first going right, then to the top.
   * @param side1 The first side that is used to determine the diagonal halo partner
   * @param side2 The second side that is used to determine the diagonal halo partner
   * @param cells Cells vector to link both cells correctly.
   */
  void linkHaloDiagonalPartner(cubeSide side1, cubeSide side2, std::vector<cell>& cells) {
    auto ghostPos = getCellGridLocation();
    ghostPos = getHaloPos(ghostPos, side1);
    ghostPos = getHaloPos(ghostPos, side2);

    std::array<double, 3> offset;
    offset = getPlaneDistance(side1, offset);
    offset = getPlaneDistance(side2, offset);

    linkPartnerUnique(ghostPos, cells, offset);
  }
};
