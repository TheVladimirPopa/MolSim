#include <gtest/gtest.h>

#include "dataStructures/Cell.h"
#include "dataStructures/LinkedCellsContainer.h"
#include "inputReader/XMLFileReader/XMLParser.h"

/**
 * Test missing deltaT parameter
 */
TEST(XMLParser, missinDeltaT) {
  EXPECT_THROW(XMLParser parser("../tests/resources/XMLTestCases/missingDeltaT.xml"), std::invalid_argument);
}
/**
 * Test missing endTime
 */
TEST(XMLParser, missingEndTime) {
  EXPECT_THROW(XMLParser parser("../tests/resources/XMLTestCases/missingEndTime.xml"), std::invalid_argument);
}

/**
 * Test missing writeOutFrequency
 */
TEST(XMLParser, missingWriteOutFrequency) {
  EXPECT_THROW(XMLParser parser("../tests/resources/XMLTestCases/missingWriteOutFrequency.xml"), std::invalid_argument);
}

/**
 * Test that the simulation parameters are initialised correctly
 */
TEST(XMLParser, initSimulation) {
  Simulation simulation{};
  XMLParser xmlParser = XMLParser("../tests/resources/XMLTestCases/Initialization1.xml");
  xmlParser.initialiseSimulationFromXML(simulation);

  EXPECT_EQ(simulation.getEndTime(), 10.0);
  EXPECT_EQ(simulation.getDeltaT(), 0.019);
  EXPECT_EQ(simulation.getWriteOutFrequency(), 20);
  EXPECT_EQ(simulation.getFilename(), "_MD.vtk");
}

/**
 * Test that the Linked Cell Container has been initialised with the correct
 * boundary types
 */
TEST(XMLParser, initLinkedCellContainerBoundary) {
  XMLParser xmlParser = XMLParser("../tests/resources/XMLTestCases/Initialization2.xml");

  LinkedCellsContainer linkedCellsContainer = *xmlParser.initialiseLinkedCellContainerFromXML().get();
  xmlParser.applyBoundariesFromXML(linkedCellsContainer);

  for (auto it : linkedCellsContainer.getBoundaries()) {
    switch (it.getSide()) {
      case CubeSide::LEFT:
        EXPECT_EQ(it.getType(), BoundaryType::REFLECT);
      case CubeSide::RIGHT:
        EXPECT_EQ(it.getType(), BoundaryType::REFLECT);
        break;
      case CubeSide::TOP:
        EXPECT_EQ(it.getType(), BoundaryType::OUTFLOW);
        break;
      case CubeSide::BOTTOM:
        EXPECT_EQ(it.getType(), BoundaryType::OUTFLOW);
        break;
      case CubeSide::FRONT:
        EXPECT_EQ(it.getType(), BoundaryType::REFLECT);
        break;
      case CubeSide::BACK:
        EXPECT_EQ(it.getType(), BoundaryType::OUTFLOW);
        break;
    }
  }

  EXPECT_EQ(linkedCellsContainer.getGridSize(), 10.0);
  std::array<double, 3> bound = {-15, -20, -5};
  EXPECT_EQ(linkedCellsContainer.getLeftLowerCorner(), bound);
  bound = {55, 30, 5};
  EXPECT_EQ(linkedCellsContainer.getRightUpperCorner(), bound);
}
/**
 * Test Sphere Input
 */
TEST(XMLParser, testSphereInput) {
  XMLParser xmlParser = XMLParser("../tests/resources/XMLTestCases/sphereTest.xml");
  std::vector<SphereArg> sphereArgs = xmlParser.extractSpheres();

  std::vector<ParticleGeneration::sphere> spheres;
  for (auto &it : sphereArgs) {
    ParticleGeneration::sphere sphere;
    sphere.position = it.getPosition();
    sphere.radius = it.getRadius();
    sphere.dimension = it.getDimension();
    sphere.velocity = it.getVelocity();
    sphere.distance = it.getDistance();
    sphere.mass = it.getMass();
    sphere.type = it.getType();

    spheres.emplace_back(sphere);
  }

  EXPECT_EQ(spheres.size(), 3);
  for (int i = 0; i < 3; i++) {
    std::array<double, 3> position{};
    unsigned int radius;
    unsigned int dimension;
    std::array<double, 3> velocity{};
    double distance;
    double mass;
    int type;

    switch (i) {
      case 0:
        position = {0.0, 0.0, 0.0};
        radius = 3;
        velocity = {0.0, 0.0, 0.0};
        dimension = 2;
        distance = 1.1225;
        mass = 1.0;
        type = 1;
        break;
      case 1:
        position = {4.0, 15.0, 0.0};
        radius = 6;
        velocity = {0.0, -10.0, 0.0};
        dimension = 2;
        distance = 1.1225;
        mass = 1.0;
        type = 2;
        break;
      case 2:
        position = {4.0, -15.0, 0.0};
        radius = 2;
        velocity = {0.0, -10.0, 0.0};
        dimension = 3;
        distance = 1.1225;
        mass = 1.0;
        type = 2;
        break;
      default:
        position = {0.0, 0.0, 0.0};
        radius = 0;
        velocity = {0.0, 0.0, 0.0};
        dimension = 0;
        distance = 0.0;
        mass = 0.0;
        type = 0;
        break;

        EXPECT_EQ(spheres[i].position, position);
        EXPECT_EQ(spheres[i].radius, radius);
        EXPECT_EQ(spheres[i].velocity, velocity);
        EXPECT_EQ(spheres[i].dimension, dimension);
        EXPECT_EQ(spheres[i].distance, distance);
        EXPECT_EQ(spheres[i].mass, mass);
        EXPECT_EQ(spheres[i].type, type);
    }
  }
}

/**
 * Test Cuboid Input
 */
TEST(XMLParser, testCuboidInput) {
  XMLParser xmlParser = XMLParser("../tests/resources/XMLTestCases/cuboidTest.xml");
  std::vector<CuboidArg> cuboidArgs = xmlParser.extractCuboid();

  std::vector<ParticleGeneration::cuboid> cuboids;
  for (auto &it : cuboidArgs) {
    ParticleGeneration::cuboid cuboid;
    cuboid.position = it.getPosition();
    cuboid.dimension = it.getDimension();
    cuboid.velocity = it.getVelocity();
    cuboid.distance = it.getDistance();
    cuboid.mass = it.getMass();
    cuboid.type = it.getType();

    cuboids.emplace_back(cuboid);
  }

  EXPECT_EQ(cuboids.size(), 3);
  for (int i = 0; i < 3; i++) {
    std::array<double, 3> position{};
    std::array<int, 3> dimension{};
    std::array<double, 3> velocity{};
    double distance;
    double mass;
    int type;

    switch (i) {
      case 0:
        position = {0.0, -30.0, 0.0};
        dimension = {4, 4, 4};
        velocity = {0.0, 10.0, 0.0};
        distance = 1.1225;
        mass = 1.0;
        type = 2;
        break;
      case 1:
        position = {80.0, 0.0, 0.0};
        dimension = {10, 10, 1};
        velocity = {-30.0, 0.0, 0.0};
        distance = 1.12;
        mass = 1.9;
        type = 1;
        break;
      case 2:
        position = {-110.0, 0.0, 0.0};
        dimension = {40, 20, 1};
        velocity = {40.0, 0.0, 0.0};
        distance = 1.9;
        mass = 1.7;
        type = 1;
        break;
      default:
        position = {0.0, 0.0, 0.0};
        dimension = {0, 0, 0};
        velocity = {0.0, 0.0, 0.0};
        distance = 0.0;
        mass = 0.0;
        type = 0;
        break;
    }

    EXPECT_EQ(cuboids[i].position, position);
    EXPECT_EQ(cuboids[i].dimension, dimension);
    EXPECT_EQ(cuboids[i].velocity, velocity);
    EXPECT_EQ(cuboids[i].distance, distance);
    EXPECT_EQ(cuboids[i].mass, mass);
    EXPECT_EQ(cuboids[i].type, type);
  }
}
/**
 * Test Thermostat input
 */
TEST(XMLParser, thermostatTest) {
  XMLParser xmlParser = XMLParser("../tests/resources/XMLTestCases/Initialization1.xml");
  LinkedCellsContainer linkedCellsContainer = *xmlParser.initialiseLinkedCellContainerFromXML().get();
  IContainer *container = &linkedCellsContainer;
  Thermostat thermostat = *xmlParser.initialiseThermostatFromXML(*container).get();

  EXPECT_EQ(thermostat.getInitialTemperature(), 40);
  EXPECT_EQ(thermostat.getTargetTemperature(), 40);
  EXPECT_EQ(thermostat.getMaxTemperatureChange(), 400);
  EXPECT_EQ(thermostat.getPeriodLength(), 1000);
  EXPECT_EQ(thermostat.getDimensionCount(), 2);
}
