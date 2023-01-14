/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>
#include <unordered_map>

class IModel;
class NewtonsLawModel;
class LennardJonesModel;
class Thermostat;
class LineProcessorCheckpoint;

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
   * Lennard Jones depth of potential well
   */
  double epsilon;

  /**
   * Lennard Jones distance at which particle-particle potential energy is 0 / size of the particle
   */
  double sigma;

  /**
   * Is true when a particle no longer is part of the simulation and waits for
   * cleanup.
   */
  bool isDeleted_;

  struct ParticleType {
    double const epsilon;
    double const sigma;
  };

  static inline std::unordered_map<int, ParticleType> typeToParametersMap{};

  /**
   * Get the ParticleType containing the Lennard-Jones parameters.
   * @param type The integer type a ParticleType was registered for
   * @return ParticleType containing the epsilon and sigma for the particle.
   * Returns the default values epsilon=5.0 and sigma=1.0 when the type is not registered.
   */
  ParticleType getLJParticleType(int type);

  /**
   * @param type Integer type of particle
   * @return Lennard Jones depth of potential well
   */
  double getLJEpsilon(int type) { return getLJParticleType(type).epsilon; }

  /**
   * @param type Integer type of particle
   * @return Sigma parameter aka. size of particle
   */
  double getLJSigma(int type) { return getLJParticleType(type).sigma; }

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

  void setX(std::array<double, 3> const &x_arg) { x = x_arg; }

  double getM() const;

  int getType() const;

  double getEpsilon() const { return epsilon; };

  double getSigma() const { return sigma; };

  bool operator==(Particle &other);

  std::string toString() const;

  // We expose access to attributes via friendships for now. These are classes
  // that are allowed to manipulate particles.
  friend class IModel;
  friend class NewtonsLawModel;
  friend class LennardJonesModel;
  friend class Thermostat;
  friend class Simulation;
  friend class LineProcessorCheckpoint;

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

  /**
   * Registers a new particle type. This is used to ensure that each particle of the same type, gets the same
   * Lennard-Jones parameters.
   * @param type Type for which Lennard-Jones parameters get registered
   * @param epsilon Lennard-Jones epsilon parameter
   * @param sigma Lennard-Jones sigma parameter
   */
  static void registerParticleType(int type, double epsilon, double sigma);

  /**
   * Adds force to the particle and subtracts force from partner particle.
   * Implements Newtons Third law.
   * @param partner The particle that gets the negative force applied
   * @param force The force that gets added to the particle and subtracted from the partner
   */
  void inline applySymmetricForce(Particle &partner, std::array<double, 3> &force);
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
