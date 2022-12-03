#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include "LinkedCellsContainer.h"
#include "utils/ArrayUtils.h"

namespace boundaryStrategy {
  struct outflow {
    static void behave() {
      // Todo: There are 2 ways to go about this:
      //  delete everything in cutoff distance
      //  Or: Wait till particle passed boundary. Then delete manually
      //  The second case avoids bugs  caused by a too big delta_t
      //  But it's a bit more expansive, as we have to relink
      // Both reflective and outflow hat to

      // Also wir brauchen ein Verhalten, was für die inneren Blöcke gemacht wird
      // und was jeweils per gelinktem auesseren Block gemacht wird.
    }
  };

  struct reflective {
    // Todo:
    static void behave() {

    }

  };

  struct periodic {

  };
}

// Imagine a cube on an XYZ grid, centered at (0,0,0). The enum describes the
// direction of a surface pointing vector with initial point (0,0,0).
enum class boundaryPlane {
  X, Y, Z
};

class ILinkedCellsBoundary {
 private:
  std::array<int, 3> cornerA;
  std::array<int, 3> cornerB;

  enum class normal { XY, XZ, YZ };

  normal plane;
  bool normalDirection;

  // Die boundary muss: über alle Cells iterieren können
  // Je cell eine Partnerzelle haben
  // Besser: Die Zellen selbst führen die Partner listen
  // Das erlaubt es uns auch später einfach Pairwise neighbor lists zu bauen
  // Cell Function: spawnGhostInMirrorCell



 public:
  std::vector<cell*> connectedCells;
  virtual void applyBoundaryLogic();

  // Boundaries are like sheets of papers with two different sides in 3D space.
  // One side influences close particles. The other side does nothing.
  // TODO: CAREFUL! If thats the case, corners in 3D and 2D are complicated
  //     FIX: method to generate boundary cube, where sheets are placed like this:
  //      Sheet      Sheet
  //     ___|__________|___    <- Sheet
  //        |          |
  //        |   Inner  |
  //        |   cells  |
  //     ___|__________|___    <- Sheet
  //        |          |

  ILinkedCellsBoundary(std::array<int, 3> cornerA, std::array<int, 3> cornerB)
      : cornerA(cornerA), cornerB(cornerB) {
    auto difference = cornerB - cornerA;

    // Either x or y or z coordinate must be the same of both corners
    if (difference[0] != 0 && difference[1] != 0 && difference[2] != 0)
      throw std::invalid_argument("Tilted boundaries are not supported.");

    auto dist = ArrayUtils::L2Norm(difference);

    if (difference[0] == dist || difference[1] == dist || difference[2] == dist)
      throw std::invalid_argument("Boundaries of width 0 are not supported.");

    if (difference[0] == 0)
      plane = normal::YZ;
    else if (difference[1] == 0)
      plane = normal::XZ;
    else
      plane = normal::XY;

    // todo: link cells
  }

  inline bool isInRange(Particle const* particle) const {
    constexpr double sigma = 1.0;  // TODO: nicht hardcoden
    constexpr double sixth_root_of_2 =
        1.1224620483093729814335330496791795162324111106139867534404095458;
    const double viewDistance = sixth_root_of_2 * sigma;

    if (plane == normal::XY)
      return std::abs(particle->getX()[2]) <= viewDistance;

    if (plane == normal::XZ)
      return std::abs(particle->getX()[1]) <= viewDistance;

    return std::abs(particle->getX()[0]) <= viewDistance;
  }

  template <typename BoundaryStrategy>
  auto applyBehavior() {
    for (auto& cell : connectedCells) {
      for (auto particle : (*cell)) {
        if (!isInRange(particle)) continue;

        BoundaryStrategy::behave();
      }
    }
  }
};
