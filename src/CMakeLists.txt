# Bundle up all files as a library so we can reuse it for MolSimTests
file(GLOB_RECURSE MY_SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

# We do want to include main into our tests later
list(REMOVE_ITEM MY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/MolSim.cpp")

add_library(MolSimLib)

target_sources(MolSimLib
  PUBLIC
    ${MY_SRC}
  )

target_include_directories(MolSimLib
  PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/libxsd
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_link_libraries(MolSimLib
  PUBLIC
    spdlog::spdlog
  )

# create make target
add_executable(MolSim "${CMAKE_CURRENT_SOURCE_DIR}/MolSim.cpp" inputReader/XMLFileReader/Args/SimulationArg.cpp inputReader/XMLFileReader/Args/SimulationArg.h inputReader/XMLFileReader/XMLFiles/input.cpp inputReader/XMLFileReader/XMLFiles/input.h inputReader/XMLFileReader/Args/LinkedCellArg.cpp inputReader/XMLFileReader/Args/LinkedCellArg.h)

# set cxx standard. You may raise this if you want.
target_compile_features(MolSim
  PRIVATE
    cxx_std_20
  )

# Directories for #include directives
target_include_directories(MolSim
  PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/libxsd
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

# Link files
target_link_libraries(MolSim
  PUBLIC
    xerces-c
    MolSimLib
  )

# depending on the compiler different flags are used
target_compile_options(MolSim
  PRIVATE
    ${global_compiler_flags}
  )