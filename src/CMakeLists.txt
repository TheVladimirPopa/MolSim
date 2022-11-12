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

# create make target
add_executable(MolSim "${CMAKE_CURRENT_SOURCE_DIR}/MolSim.cpp")

# set cxx standard. You may raise this if you want.
target_compile_features(MolSim
  PRIVATE
    cxx_std_20
  )

target_include_directories(MolSim
  PUBLIC
    ${PROJECT_SOURCE_DIR}/libs/libxsd
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_link_libraries(MolSim
  PUBLIC
    xerces-c
    MolSimLib
)

# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
target_compile_options(MolSim
  PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    # disable some spam warnings for icpc...
    $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
  )