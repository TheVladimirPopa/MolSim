# Collect test files
file(GLOB_RECURSE MY_TESTS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        )

add_executable(MolSimTests ${MY_TESTS})

target_link_libraries(MolSimTests
        PRIVATE
        gtest_main
        gmock
        xerces-c
        MolSimLib
        )

# Consider tests/ as directory to look for files for #include directives.
target_include_directories(MolSimTests
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

# Standard: C++ 20
target_compile_features(MolSimTests PRIVATE cxx_std_20)

# Standard compile flags
target_compile_options(MolSimTests
        PRIVATE
        ${global_compiler_flags}
        )

include(GoogleTest)
include(CTest)
gtest_discover_tests(MolSimTests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})