/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>

class IModel;
class NewtonsLawModel;
class LennardJonesModel;

class Particle {
 private:
  /**
   * Position of the particle
   */
  std::array<double, 3> x;

  /**
   * Velocity of the particle
   */
  std::array<double, 3> v;

  /**
   * Force effective on this particle
   */
  std::array<double, 3> f;

  /**
   * Force which was effective on this particle
   */
  std::array<double, 3> old_f;

  /**
   * Mass of this particle
   */
  double m;

  /**
   * Type of the particle. Use it for whatever you want (e.g. to separate
   * molecules belonging to different bodies, matters, and so on)
   */
  int type;

  /**
   * Is true when a particle no longer is part of the simulation and waits for
   * cleanup.
   */
  bool isDeleted_;

 public:
  explicit Particle(int type = 0);

  Particle(const Particle &other);

  Particle(
      // for visualization, we need always 3 coordinates
      // -> in case of 2d, we use only the first and the second
      std::array<double, 3> x_arg, std::array<double, 3> v_arg = {0.0, 0.0, 0.0}, double m_arg = 1.0, int type = 0);

  virtual ~Particle();

  const std::array<double, 3> &getX() const { return x; }

  const std::array<double, 3> &getV() const { return v; }

  const std::array<double, 3> &getF() const { return f; }

  const std::array<double, 3> &getOldF() const { return old_f; }

  void setX(std::array<double, 3> const&x_arg) { x = x_arg; }

  double getM() const;

  int getType() const;

  bool operator==(Particle &other);

  std::string toString() const;

  // We expose access to attributes via friendships for now. These are classes
  // that are allowed to manipulate particles.
  friend class IModel;
  friend class NewtonsLawModel;
  friend class LennardJonesModel;

  /**
   * Move current forces on particle to old_f and set f to 0-vector so we can
   * start a new iteration.
   */
  void updateForces();

  /**
   * true iff a particle no longer is part of the simulation and waits for
   * cleanup.
   */
  bool isDeleted() { return isDeleted_; }

  /**
   * Deleted particle by setting it's deletion state.
   */
  void deleteParticle() { isDeleted_ = true; }
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
