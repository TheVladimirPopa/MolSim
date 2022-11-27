#pragma once

#include "IContainer.h"
class LinkedCellsContainer : public IContainer {
 private:
  enum cellType { inner, boundary, halo };

  struct cell {
    std::vector<Particle *> particles{};
    cellType type = inner;

    explicit cell(cellType t) : type{t} {}
  };

  std::vector<Particle> particlesVector;

  std::vector<cell> cells;

  double gridSize;

  std::array<double, 3> leftLowerCorner;
  std::array<double, 3> rightUpperCorner;

  std::array<size_t, 3> dimensions{};

  size_t getCellIndexOfPosition(std::array<double, 3> &position);

  size_t getVectorIndexFromCord(size_t x, size_t y, size_t z);

 public:
  LinkedCellsContainer(double cellSize, std::array<double, 3> &leftLowerBound,
                       std::array<double, 3> &rightUpperBound);

  ~LinkedCellsContainer() override = default;
  ;

  void forEach(std::function<void(Particle &)> &unaryFunction) override;

  void forEachPair(
      std::function<void(Particle &, Particle &)> &binaryFunction) override;

  void reserve(size_t amount) override;

  size_t capacity() override;

  size_t size() override;
};
