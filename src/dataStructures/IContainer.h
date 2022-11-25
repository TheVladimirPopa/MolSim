#include <functional>

#include "Particle.h"

class IContainer {
 public:
  /**
   * Applies a function to all particles in the container
   * @param unaryFunction A function taking a particle reference and returning
   * nothing
   */
  virtual void forEach(std::function<void(Particle &)> &unaryFunction) = 0;

  /**
   * Applies a function to all particle pairs in the container
   * @param binaryFunction A function taking two particle references and
   * returning nothing
   * @note A pair is considered without order, so if the function got applied to
   * (p1,p2), it won't get applied to (p2,p1). Also pairs with the same particle
   * are not considered
   */
  virtual void forEachPair(
      std::function<void(Particle &, Particle &)> &binaryFunction) = 0;

  virtual void reserve(size_t amount) = 0;

  virtual void emplace_back(std::array<double, 3> x_arg,
                            std::array<double, 3> v_arg = {0.0, 0.0, 0.0},
                            double m_arg = 1.0, int type = 0) = 0;
  virtual size_t capacity() = 0;
  virtual size_t size() = 0;

  virtual ~IContainer() = default;
};
