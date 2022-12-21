#pragma once

#include <array>
#include <iostream>

class LinkedCellArg {
 private:
  double cellSize;
  std::array<double, 3> leftLowerBound;
  std::array<double, 3> rightUpperBound;
  std::string bound_left;
  std::string bound_right;
  std::string bound_top;
  std::string bound_bottom;
  std::string bound_front;
  std::string bound_back;

 public:
  LinkedCellArg(double cellSize, const std::array<double, 3> &leftLowerBound,
                const std::array<double, 3> &rightUpperBound);

  [[nodiscard]] double getCellSize() const;

  [[nodiscard]] const std::array<double, 3> &getLeftLowerBound() const;

  [[nodiscard]] const std::array<double, 3> &getRightUpperBound() const;

  [[nodiscard]] const std::string &getBoundLeft() const;

  void setBoundLeft(const std::string &boundLeft);

  [[nodiscard]] const std::string &getBoundRight() const;

  void setBoundRight(const std::string &boundRight);

  [[nodiscard]] const std::string &getBoundTop() const;

  void setBoundTop(const std::string &boundTop);

  [[nodiscard]] const std::string &getBoundBottom() const;

  void setBoundBottom(const std::string &boundBottom);

  [[nodiscard]] const std::string &getBoundFront() const;

  void setBoundFront(const std::string &boundFront);

  [[nodiscard]] const std::string &getBoundBack() const;

  void setBoundBack(const std::string &boundBack);
};
