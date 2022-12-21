#include "dataStructures/VectorContainer.h"
#include "gtest/gtest.h"
#include "model/Thermostat.h"
#include "utils/ArrayUtils.h"

using v3d = std::array<double, 3>;

/**
 * Make sure the total kinetic energy of an system is calculated correctly
 */
TEST(Thermostat, ComputeEnergy) {
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{1., 2., 2.}, 3.0, 1);

  // E_kin= 3 * (1²+2²+2²)/2 = 3*9/2 = 27/2 = 13.5
  Thermostat thermostat{container, 0., 0., 0., 0};
  EXPECT_EQ(thermostat.computeEnergy(), 13.5);

  container.emplace_back(v3d{0., 3., 0.}, v3d{5., 3., 4.}, 1.0, 1);  // Ekin=25
  container.emplace_back(v3d{0., 5., 0.}, v3d{1., 0., 0.}, 15.0,
                         1);  // Ekin=7.5

  EXPECT_EQ(thermostat.computeEnergy(), 13.5 + 25. + 7.5);
}

/**
 * Make sure the temperature is calculated correctly
 */
TEST(Thermostat, ComputeTemperature) {
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{1., 2., 2.}, 3.0,
                         1);                                         // Ekin=13.5
  container.emplace_back(v3d{0., 3., 0.}, v3d{5., 3., 4.}, 1.0, 1);  // Ekin=25
  container.emplace_back(v3d{0., 5., 0.}, v3d{1., 0., 0.}, 13.0,
                         1);  // Ekin=6.5
  // E_kin= 45

  Thermostat thermostat{container, 0., 0., 0., 0, 3};
  EXPECT_EQ(thermostat.getCurrentTemperature(), 2. * 45. / (3 * 3));

  Thermostat thermostat2d{container, 0., 0., 0., 0, 2};
  EXPECT_EQ(thermostat2d.getCurrentTemperature(), 2. * 45. / (3 * 2));
}

/**
 * Make sure the velocities of all particles get scaled to reach target
 * temperature
 */
TEST(Thermostat, VelocitiesScaledCorrectly) {
  std::array<v3d, 3> velocities{v3d{1., 1., 1.}, v3d{0., 10., 0.}, v3d{1., 0., 0.}};
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{1., 1., 1.}, 2.0, 1);   // Ekin=3
  container.emplace_back(v3d{0., 3., 0.}, v3d{0., 10., 0.}, 1.0, 1);  // Ekin=50
  container.emplace_back(v3d{0., 5., 0.}, v3d{1., 0., 0.}, 2.0, 1);   // Ekin=1
  // Ekin= 54
  // Tcurr= 2*54/(3*3)=12
  Thermostat thermostatDoNothing{container, 0., 12., 100., 0, 3};
  ASSERT_EQ(thermostatDoNothing.getCurrentTemperature(), 12.);

  thermostatDoNothing.applyThermostat();

  EXPECT_EQ(thermostatDoNothing.getCurrentTemperature(), 12.);
  for (auto &p : container.getVector()) {
    EXPECT_TRUE((p.getV() == velocities[0]) || (p.getV() == velocities[1]) || (p.getV() == velocities[2]));
  }

  Thermostat thermostatHalfVelocity{container, 0., 3., 100., 0, 3};
  thermostatHalfVelocity.applyThermostat();
  EXPECT_EQ(thermostatHalfVelocity.getCurrentTemperature(), 3.);
  for (auto &p : container.getVector()) {
    EXPECT_TRUE((p.getV() == 0.5 * velocities[0]) || (p.getV() == 0.5 * velocities[1]) ||
                (p.getV() == 0.5 * velocities[2]));
  }

  thermostatDoNothing.applyThermostat();

  EXPECT_EQ(thermostatDoNothing.getCurrentTemperature(), 12.);
  for (auto &p : container.getVector()) {
    EXPECT_TRUE((p.getV() == velocities[0]) || (p.getV() == velocities[1]) || (p.getV() == velocities[2]));
  }
}

/**
 * Make sure the temperature change is below the maxTemperatureChange
 */
TEST(Thermostat, StepwiseCoolingAndHeating) {
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{0., 4., 3.}, 2.0, 1);
  container.emplace_back(v3d{0., 3., 0.}, v3d{0., -4., 3.}, 1.0, 1);
  container.emplace_back(v3d{0., 5., 0.}, v3d{-3., 0., 6.}, 5.0, 2);
  container.emplace_back(v3d{0., 5., 0.}, v3d{-5., 0., 0.}, 3., 3);
  container.emplace_back(v3d{0., 5., 0.}, v3d{5., 3., 4.}, 6.0, 4);

  Thermostat thermostat{container, 0., 20., 5., 0, 3};

  for (int i = 0; i < (45 - 20) / 5; ++i) {
    EXPECT_DOUBLE_EQ(thermostat.getCurrentTemperature(), 45 - (i * 5));
    thermostat.applyThermostat();
  }
  EXPECT_DOUBLE_EQ(thermostat.getCurrentTemperature(), 20);
  thermostat.applyThermostat();
  EXPECT_DOUBLE_EQ(thermostat.getCurrentTemperature(), 20);

  Thermostat thermostat40{container, 0., 40., 7., 0, 3};

  for (int i = 0; i < (41 - 20) / 7; ++i) {
    EXPECT_DOUBLE_EQ(thermostat40.getCurrentTemperature(), 20 + (i * 7));
    thermostat40.applyThermostat();
  }
  EXPECT_DOUBLE_EQ(thermostat40.getCurrentTemperature(), 40);
  thermostat40.applyThermostat();
  EXPECT_DOUBLE_EQ(thermostat40.getCurrentTemperature(), 40);
}

/**
 * Test if the initialTemperature is set correctly when the particles are
 * already moving
 */
TEST(Thermostat, InitialTemperature) {
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{0., 1., 0.}, 2.0, 1);
  container.emplace_back(v3d{0., 3., 0.}, v3d{0., 1., 0.}, 1.0, 1);
  container.emplace_back(v3d{0., 5., 0.}, v3d{-1., 0., 6.}, 5.0, 2);
  container.emplace_back(v3d{0., 5., 0.}, v3d{-1., 0., 0.}, 3., 3);
  container.emplace_back(v3d{0., 5., 0.}, v3d{0, 3., 4.}, 6.0, 4);
  Thermostat thermostat{container, 50., 0., 0., 10, 3};

  thermostat.initializeTemperature();
  EXPECT_DOUBLE_EQ(thermostat.getCurrentTemperature(), 50.);
}

/**
 * Test if the initial temperature is set correctly, when all particles have
 * zero velocity
 */
TEST(Thermostat, InitialTemperatureBrownian) {
  VectorContainer container{};
  container.emplace_back(v3d{0., 0., 0.}, v3d{0., 0., 0.}, 2.0, 1);
  container.emplace_back(v3d{0., 3., 0.}, v3d{0., 0., 0.}, 1.0, 1);
  container.emplace_back(v3d{0., 5., 0.}, v3d{0., 0., 0.}, 5.0, 2);
  container.emplace_back(v3d{0., 5., 0.}, v3d{0., 0., 0.}, 3., 3);
  container.emplace_back(v3d{0., 5., 0.}, v3d{0, 0., 0.}, 6.0, 4);
  Thermostat thermostat{container, 50., 0., 0., 10, 3};

  thermostat.initializeTemperature();
  EXPECT_DOUBLE_EQ(thermostat.getCurrentTemperature(), 50.);
}
