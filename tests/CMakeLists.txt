# Include googletest:
    # src https://github.com/google/googletest/blob/main/googletest
    include(FetchContent)
    FetchContent_Declare(
            googletest
            # Current commit id is version 1.12.1
            # https://github.com/google/googletest/releases/tag/release-1.12.1
            URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
# -------

# Collect test files
file(GLOB_RECURSE MY_TESTS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    )

add_executable(MolSim_Tests ${MY_TESTS})

# Standard: C++ 20
target_compile_features(MolSim_Tests PRIVATE cxx_std_20)

# Standard compile flags
target_compile_options(MolSim_Tests
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
        )

# Link gtest
target_link_libraries(MolSim_Tests gtest_main)
#add_test(NAME example_test COMMAND example)
