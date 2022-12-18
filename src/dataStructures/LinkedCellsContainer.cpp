#include "LinkedCellsContainer.h"

#include "LinkedCellsBoundary.h"
#include "utils/ArrayUtils.h"

LinkedCellsContainer::LinkedCellsContainer(double cellSize, std::array<double, 3> &leftLowerBound,
                                           std::array<double, 3> &rightUpperBound)
    : cells{},
      gridSize{cellSize},
      leftLowerCorner{leftLowerBound},
      rightUpperCorner{rightUpperBound},
      cubeSize{rightUpperCorner - leftLowerCorner} {
  std::array<double, 3> boundingBoxDim{cubeSize};
  size_t numCells{1};
  for (int i = 0; i < 3; ++i) {
    dimensions[i] = static_cast<size_t>(floor(boundingBoxDim[i] / gridSize)) + 2;
    // Make sure each dimension has at least one boundary cell
    if (dimensions[i] == 2) {
      dimensions[i] = 3;
    }
    numCells *= dimensions[i];
  }

  cells.reserve(numCells);

  // Instantiate all the cells with their specified type
  for (size_t z = 0; z < dimensions[2]; ++z) {
    for (size_t y = 0; y < dimensions[1]; ++y) {
      for (size_t x = 0; x < dimensions[0]; ++x) {
        if (x == 0 or y == 0 or z == 0 or x == dimensions[0] - 1 or y == dimensions[1] - 1 or z == dimensions[2] - 1) {
          cells.emplace_back(CellType::halo, cells.size(), &dimensions, &cubeSize);
        } else if (x == 1 or y == 1 or z == 1 or x == dimensions[0] - 2 or y == dimensions[1] - 2 or
                   z == dimensions[2] - 2) {
          cells.emplace_back(CellType::boundary, cells.size(), &dimensions, &cubeSize);

        } else {
          cells.emplace_back(CellType::inner, cells.size(), &dimensions, &cubeSize);
        }
      }
    }
  }

  // Precompute the indexOffsets, which are used to quickly determine the
  // adjacent cells
  int indexAdjacentArray = 0;
  indexOffsetAdjacent[indexAdjacentArray++] = 0;
  indexOffsetAdjacent[indexAdjacentArray++] = 1;
  for (int x = -1; x < 2; ++x) {
    indexOffsetAdjacent[indexAdjacentArray++] = dimensions[0] + x;
  }
  for (int y = -1; y < 2; ++y) {
    for (int x = -1; x < 2; ++x) {
      indexOffsetAdjacent[indexAdjacentArray++] =
          static_cast<int>(dimensions[1] * dimensions[0]) + x + (y * dimensions[0]);
    }
  }
}

size_t LinkedCellsContainer::getVectorIndexFromCoord(size_t x, size_t y, size_t z) {
  return x + (y * dimensions[0]) + (z * dimensions[0] * dimensions[1]);
}

size_t LinkedCellsContainer::getCellIndexOfPosition(std::array<double, 3> const &position) {
  std::array<double, 3> positionInBox = position - leftLowerCorner;
  std::array<int, 3> indexInBox{};

  for (int i = 0; i < 3; ++i) {
    //+1 has to be added because of the 0 cell in each dimension is a halo cell
    indexInBox[i] = static_cast<int>(floor(positionInBox[i] / gridSize)) + 1;

    // If the position is far outside the domain bounding box  map, set to
    // according halo box
    if (indexInBox[i] < 0) {
      indexInBox[i] = 0;
    }
    // If the particle is currently placed in a halo cell, it has to be tested
    // if it is still in the domain box, since the cell size of the last cell
    // may be bigger
    if (indexInBox[i] >= static_cast<int>(dimensions[i]) - 2) {
      if (position[i] > rightUpperCorner[i]) {
        // Place it in the halo cell
        indexInBox[i] = static_cast<int>(dimensions[i]) - 1;
      } else {
        // Place it in the boundary cell
        indexInBox[i] = static_cast<int>(dimensions[i]) - 2;
      }
    }
  }

  return getVectorIndexFromCoord(indexInBox[0], indexInBox[1], indexInBox[2]);
}

std::vector<std::reference_wrapper<Particle>> LinkedCellsContainer::getNeighbors(Particle &particle) {
  std::vector<std::reference_wrapper<Particle>> neighbors;

  size_t gridCell = getCellIndexOfPosition(particle.getX());
  for (size_t indexOffset : indexOffsetAdjacent) {
    // Special case to match particles within one cell
    if (indexOffset == 0) {
      for (auto &&particleIndex : cells[gridCell].particles) {
        auto &neighborParticle = particlesVector[particleIndex];

        if (&particle != &neighborParticle) neighbors.push_back(neighborParticle);
      }
    } else {
      // Loop so the particles of each of the two cells and match them
      size_t partnerCellIndex = gridCell + indexOffset;
      if (partnerCellIndex < 0 || partnerCellIndex >= cells.size()) continue;

      for (auto &&cellBParticle : cells[partnerCellIndex].particles) {
        auto &neighborParticle = particlesVector[cellBParticle];

        if (&particle != &neighborParticle) neighbors.push_back(neighborParticle);
      }
    }
  }

  return neighbors;
}

void LinkedCellsContainer::forEachNeighbor(Particle &particle,
                                           std::function<void(Particle &, Particle &)> &binaryFunction) {
  std::vector<std::reference_wrapper<Particle>> neighbors = getNeighbors(particle);

  for (auto &neighbor : neighbors) binaryFunction(particle, neighbor);
}

void LinkedCellsContainer::forEach(std::function<void(Particle &)> &unaryFunction) {
  for (auto &p : particlesVector) {
    if (!p.isDeleted()) unaryFunction(p);
  }
}

void LinkedCellsContainer::forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) {
  recalculateStructure();
  applyBoundaries();

  if (hasPeriodicBoundaries) {
    // TODO: SAVE THIS CALL BY MANUALLY RELINKING PARTICLES!
    recalculateStructure();
    applyPeriodicForces(binaryFunction);
  }

  for (size_t index = 0; index < cells.size(); ++index) {
    if (cells[index].type == CellType::halo) {
      continue;
    }
    for (size_t indexOffset : indexOffsetAdjacent) {
      // Special case to match particles within one cell
      if (indexOffset == 0) {
        auto &particles = cells[index].particles;
        for (auto first = particles.begin(); first != particles.end(); ++first) {
          for (auto second = first; second != particles.end(); ++second) {
            // Don't run the function on a pair consisting of the same particle
            if (*second == *first) continue;
            binaryFunction(particlesVector[*first], particlesVector[*second]);
          }
        }

      } else {
        // Loop so the particles of each of the two cells and match them
        for (auto cellAParticle : cells[index].particles) {
          for (auto cellBParticle : cells[index + indexOffset].particles) {
            binaryFunction(particlesVector[cellAParticle], particlesVector[cellBParticle]);
          }
        }
      }
    }
  }
}

void LinkedCellsContainer::reserve(size_t amount) { particlesVector.reserve(amount); }
size_t LinkedCellsContainer::capacity() { return particlesVector.capacity(); }

size_t LinkedCellsContainer::size() {
  return std::count_if(particlesVector.begin(), particlesVector.end(), [](Particle &p) { return !p.isDeleted(); });
}

void LinkedCellsContainer::emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg,
                                        int type) {
  particlesVector.emplace_back(x_arg, v_arg, m_arg, type);
  size_t index = getCellIndexOfPosition(x_arg);
  cells[index].particles.push_back(particlesVector.size() - 1);
}

void LinkedCellsContainer::recalculateStructure() {
  // Loop through each cell
  for (size_t cellIndex = 0; cellIndex < cells.size(); ++cellIndex) {
    // Loop through each particle of that cell
    auto &particles = cells[cellIndex].particles;
    for (auto it = particles.begin(); it != particles.end();) {
      auto pos = particlesVector[(*it)].getX();
      size_t correctIndex = getCellIndexOfPosition(pos);
      if (correctIndex != cellIndex) {
        // Delete particle and push it into correct cell, if it is currently in
        // the wrong cell
        cells[correctIndex].particles.push_back(*it);
        it = particles.erase(it);
      } else {
        ++it;
      }
    }
  }
}

[[maybe_unused]] void LinkedCellsContainer::setBoundaries(std::vector<std::pair<CubeSide, BoundaryType>> sideAndType) {
  // Ensure that boundary[0] always is the LEFT one, etc.
  std::sort(sideAndType.begin(), sideAndType.end(), [](std::pair<CubeSide, BoundaryType> lhs, auto rhs) {
    return static_cast<int>(lhs.first) < static_cast<int>(rhs.first);
  });

  for (auto [side, type] : sideAndType) {
    boundaries.emplace_back(side, type, cells, &particlesVector, dimensions, &leftLowerCorner, &rightUpperCorner);
  }

  for (auto &boundary : boundaries) {
    if (boundary.getType() == BoundaryType::PERIODIC) {
      hasPeriodicBoundaries = true;
      break;
    }
  }

  linkBoundaryToHaloCells();
}

/**
 * Essentially checks whether periodic boundaries are matched correctly. Takes a set of boundaries and returns "leading
 * sides". A side is a leading side, if it is representative of a matching pair (E.g. LEFT boundary + RIGHT boundary are
 * a matching pair), where the side has the lower casted numerical index. (LEFT has index of 0, vs. RIGHT with 1. So
 * LEFT will always represent the pair LEFT+RIGHT.)
 * @param boundaries Boundaries to check, though only periodic boundaries are considered
 * @return Leading sides, representing periodic boundary pairs as defined above
 */
std::vector<CubeSide> getLeadingSides(std::vector<LinkedCellsBoundary> boundaries) {
  std::vector<CubeSide> leadingSides;

  for (auto &boundary : boundaries) {
    if (boundary.getType() != BoundaryType::PERIODIC) continue;

    CubeSide oppositeSide = cell::getOppositeSide(boundary.getSide());

    auto matchBoundaryIt =
        std::find_if(boundaries.begin(), boundaries.end(), [&oppositeSide](LinkedCellsBoundary bound) {
          return bound.getSide() == oppositeSide && bound.getType() == BoundaryType::PERIODIC;
        });

    // If e.g. for the LEFT periodic boundary, RIGHT is missing, we throw.
    if (matchBoundaryIt == boundaries.end()) throw std::invalid_argument("Periodic boundaries miss-matched.");

    // If we match LEFT and RIGHT, we store LEFT as representative for the pair.
    int tmpSideInt = static_cast<int>(boundary.getSide()) % 2;
    CubeSide leadingSide = (tmpSideInt == 0) ? boundary.getSide() : static_cast<CubeSide>(tmpSideInt - 1);

    if (std::find(leadingSides.begin(), leadingSides.end(), leadingSide) == leadingSides.end())
      leadingSides.push_back(leadingSide);
  }

  return leadingSides;
}

void LinkedCellsContainer::linkBoundaryToHaloCells() {
  // Periodic boundaries musst occur in pairs. E.g. a LEFT + RIGHT periodic boundary.

  auto &boundaries = this->boundaries;
  std::vector<CubeSide> leadingSides = getLeadingSides(boundaries);

  // No pair of periodic boundaries. Don't link anything.
  if (leadingSides.size() == 0) return;

  // More than 3 pairs of periodic boundaries must be an error.
  if (leadingSides.size() > 3)
    throw std::logic_error("There are more than 3 pairs of periodic boundaries on container.");

  // Simple case: Per periodic boundary: Link boundary cells to the halo cells on the opposite side of the container.
  for (auto &boundary : boundaries) {
    if (boundary.getType() != BoundaryType::PERIODIC) continue;

    for (cell *boundaryCell : boundary.getConnectedCells()) {
      boundaryCell->linkParallelPeriodicPartner(cell::getOppositeSide(boundary.getSide()), cells);
    }
  }

  // Special case: Link corners between 2 periodic boundaries (2D diagonal partners)
  if (leadingSides.size() >= 2) {
    // Iterate over every pair of boundaries
    for (auto it1 = boundaries.begin(); it1 < boundaries.end(); it1++) {
      if ((*it1).getType() != BoundaryType::PERIODIC) continue;
      for (auto it2 = it1 + 1; it2 < boundaries.end(); it2++) {
        if ((*it2).getType() != BoundaryType::PERIODIC or it1 == it2) continue;

        CubeSide opSide1 = cell::getOppositeSide((*it1).getSide());
        CubeSide opSide2 = cell::getOppositeSide((*it2).getSide());

        // If and only if a cell boundary cell is shared between 2 periodic boundaries, it has a diagonal halo partner
        for (cell *boundaryCell : (*it1).getConnectedCells()) {
          auto const &otherCells = (*it2).getConnectedCells();
          bool isShared = std::find(otherCells.begin(), otherCells.end(), boundaryCell) != otherCells.end();

          if (isShared) boundaryCell->linkDiagonalPeriodicPartner(opSide1, opSide2, cells);
        }
      }
    }
  }

  // Special, special case: 3 pairs of periodic boundaries. Cube corners must be linked. (3d diagonal partners)
  if (leadingSides.size() == 3) {
    linkCornerBoundaryCells();
  }
}

void LinkedCellsContainer::linkCornerBoundaryCells() {
  // If the cube is surrounded by periodic boundaries, we must also link the 8 inner corners with the 8 outer halo
  // cells
  auto cubeSize = rightUpperCorner - leftLowerCorner;

  for (int i = 0b000; i <= 0b111; i++) {
    std::array<unsigned int, 3> boundaryCellPos = {
        (i & 0b001) == 0 ? 1 : dimensions[0] - 2,
        (i & 0b010) == 0 ? 1 : dimensions[1] - 2,
        (i & 0b100) == 0 ? 1 : dimensions[2] - 2,
    };

    std::array<unsigned int, 3> oppositeHaloPos = {
        (i & 0b001) == 0 ? dimensions[0] - 1 : 0,
        (i & 0b010) == 0 ? dimensions[1] - 1 : 0,
        (i & 0b100) == 0 ? dimensions[2] - 1 : 0,
    };

    std::array<double, 3> offset = {
        (i & 0b001) == 0 ? cubeSize[0] : -cubeSize[0],
        (i & 0b010) == 0 ? cubeSize[1] : -cubeSize[1],
        (i & 0b100) == 0 ? cubeSize[2] : -cubeSize[2],
    };

    cells[getVectorIndexFromCoord(boundaryCellPos[0], boundaryCellPos[1], boundaryCellPos[2])].linkPartnerUnique(
        oppositeHaloPos, cells, offset);
  }
}

void LinkedCellsContainer::applyPeriodicForces(std::function<void(Particle &, Particle &)> &binaryFunction) {
  for (cell &halo : cells) {
    if (halo.type != CellType::halo) continue;

    // For each halo cell: move connected boundary cell particles into halo cells, apply function, move back
    for (cell::PeriodicPartner &partner : halo.periodicPartners) {
      for (size_t pIndex : partner.pCell->particles) {
        auto &particle = particlesVector[pIndex];

        // Move particle
        auto originalLocation = particle.getX();
        particle.setX(originalLocation + partner.offset);

        // Apply function / forces
        forEachNeighbor(particle, binaryFunction);

        // Revert location of particle
        // (Copy, not subtract, to avoid bad conditioning of subtraction)
        particle.setX(originalLocation);
      }
    }
  }
}