# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Create test executable
add_executable(snirf_tests
    snirf_test.cpp
)

# Define SNIRF_USE_GTEST for Google Test builds
target_compile_definitions(snirf_tests PRIVATE SNIRF_USE_GTEST)

# Link against the snirf library and Google Test
target_link_libraries(snirf_tests
    PRIVATE
        snirf
        GTest::gtest_main
)

# Discover tests for CTest
include(GoogleTest)
gtest_discover_tests(snirf_tests)
