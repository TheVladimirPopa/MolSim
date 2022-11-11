# Collect test files
file(GLOB_RECURSE MY_SRC
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
        )
message(STATUS "foo include dir: ${MY_SRC}")

file(GLOB_RECURSE MY_TESTS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )
message(STATUS "foo include dir: ${MY_TESTS}")

add_executable(MolSim_Tests ${MY_TESTS})

target_link_libraries(MolSim_Tests
  PRIVATE
    gtest_main
    xerces-c
    MolSimLib
  )

# Standard: C++ 20
target_compile_features(MolSim_Tests PRIVATE cxx_std_20)

# Standard compile flags
target_compile_options(MolSim_Tests
  PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    # disable some spam warnings for icpc...
    $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
  )

include(GoogleTest)
include(CTest)
gtest_discover_tests(MolSim_Tests)