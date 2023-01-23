#pragma once
#include <fstream>
#include <functional>

#include "MembraneMolecule.h"
#include "Particle.h"
/**  \image html runtime.png width=900px
 *    \image latex runtime.png "Runtimes comparison" width = [goodwidth]cm
 */
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
  virtual void forEachPair(std::function<void(Particle &, Particle &)> &binaryFunction) = 0;

  /**
   * Reserves storage and resizes the underlying datastructures
   * @param amount Specifies the count of Particles that should fit in the
   * datastructure
   * @note You don't have to use it, but it can gain performance if you call it
   * once at the start with the right particle count
   */
  virtual void reserve(size_t amount) = 0;

  /**
   * Constructs a particle in-place and puts it into the datastructure
   * @param x_arg The position vector of the particle
   * @param v_arg The velocity vector of the particle
   * @param m_arg The mass of the particle
   * @param type The type of the particle
   */
  virtual void emplace_back(std::array<double, 3> x_arg, std::array<double, 3> v_arg = {0.0, 0.0, 0.0},
                            double m_arg = 1.0, int type = 0) = 0;

  /**
   * Inserts a external particle into the container.
   * @param particle
   */
  virtual void push_back(Particle &particle) = 0;

  /**
   * Returns the number of particles that can be held in currently allocated
   * storage
   * @return the number of particles that can be held in currently allocated
   * storage
   */
  virtual size_t capacity() = 0;

  /**
   * Returns the number of particles in the container
   * @return the number of particles
   */
  virtual size_t size() = 0;

  /**
   * Attaches a membrane molecule to the container.
   * For now (worksheet 5) only Membrane molecules are supported.
   * @param membrane The membrane that will be attached to the container.
   */
  virtual void push_back(MembraneMolecule membrane) = 0;

  /**
   * Returns a reference to a vector containing the particles of the container.
   * @return A vector containing particles
   */
  virtual std::vector<Particle> &getParticlesRef() = 0;

  /**
   * @return True, if the container has molecules and support molecules. Otherwise false.
   */
  virtual bool containsMolecules() { return false; };

  /**
   * @return Reference to membrane vector
   */
  virtual std::vector<MembraneMolecule> &getMoleculesVectorRef() { throw std::runtime_error("Not implemented."); }

  virtual ~IContainer() = default;
};
