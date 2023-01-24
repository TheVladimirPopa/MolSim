#pragma once

enum class InputType { File, XML };

enum class ContainerType { VECTOR = 0, LINKED_CELLS = 1 };

enum class ModelType { NewtonsLaw, LennardJones };

enum class WriterType { NoWriter, VTKWriter };

enum class SimTypeDeprecated { Single, Cuboid, Sphere };