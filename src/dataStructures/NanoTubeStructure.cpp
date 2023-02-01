#include "NanoTubeStructure.h"

NanoTubeStructure::NanoTubeStructure(std::array<size_t, 3> dimensions, double stiffness, double bondLength,
                                     double cutOff, std::vector<Particle>& particles)
    : MembraneStructure(dimensions, stiffness, bondLength, cutOff, particles) {}

void NanoTubeStructure::applyForce(Particle& p1, Particle& p2)  {
  if (p2.getId() < p1.getId()) return applyForce(p2, p1);
  size_t diff = p2.getId() - p1.getId();
  size_t width = dimensions[dimWidth];

  bool isNotAtRightBorder = (p1.getId() - startIndex) % width + 1 != width;
  bool isNotAtLeftBorder = (p1.getId() - startIndex) % width != 0;

  if ((diff == 1 && isNotAtRightBorder) || diff == width || (!isNotAtLeftBorder && diff == (width - 1))) return applyDirectForce(p1, p2);

  if (((diff == width - 1) && isNotAtLeftBorder) || ((diff == width + 1) && isNotAtRightBorder) ||
      (!isNotAtLeftBorder && diff == (width + width - 1)) || (!isNotAtRightBorder && diff == 1))
    return applyDiagonalForce(p1, p2);

  return applyRepulsiveForce(p1, p2);
}

