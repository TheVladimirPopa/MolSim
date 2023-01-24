#pragma once

/// Type of input
enum class InputType { File, XML };

/// Type of the used container
enum class ContainerType { VECTOR = 0, LINKED_CELLS = 1 };

/// Type of the used model
enum class ModelType { NewtonsLaw, LennardJones };

/// Type of the used writer
enum class WriterType { NoWriter, VTKWriter };

/// Type of the file input (deprecated, use xml)
enum class SimTypeDeprecated { Single, Cuboid, Sphere };