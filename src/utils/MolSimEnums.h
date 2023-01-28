#pragma once

/// Type of input
enum class InputType { File, XML };

/// Type of the used container
enum class ContainerType {
  VECTOR = 0,
  LINKED_CELLS = 1,
  LINKED_CELLS_LOCKS = 2,
  LINKED_CELLS_COLOURING_SINGLE = 3,
  LINKED_CELLS_COLOURING_MULTIPLE = 4
};

/// Type of the used model
enum class ModelType { NewtonsLaw, LennardJones, SmoothedLennardJones };

/// Type of the used writer
enum class WriterType { NoWriter, VTKWriter };

/// Type of the file input (deprecated, use xml)
enum class SimTypeDeprecated { Single, Cuboid, Sphere };