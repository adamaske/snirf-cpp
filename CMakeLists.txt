cmake_minimum_required(VERSION 3.14)

project(snirfcpp VERSION 1.0 LANGUAGES CXX)


# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Determine if this is a standalone build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SNIRF_STANDALONE ON)
else()
    set(SNIRF_STANDALONE OFF)
endif()

# Options
option(SNIRF_BUILD_TESTS "Build tests" OFF)
option(SNIRF_BUILD_EXAMPLES "Build examples" OFF)
option(SNIRF_BUILD_DEV_EXECUTABLE "Build development executable" ${SNIRF_STANDALONE})

# Define the static library
add_library(snirf STATIC
    "src/snirf.cpp"
    "include/snirf/snirf.h"
    "include/snirf/SNIRFFactory.h"
    "include/snirf/snirftype.h"
    "src/SNIRFFactory.cpp"
    "include/snirf/utils.h"
 "include/snirf/SNIRFError.h")

# Include directories
target_include_directories(snirf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(HDF5 REQUIRED COMPONENTS CXX)

set(HIGHFIVE_FIND_HDF5 OFF CACHE BOOL "" FORCE)  # We already found HDF5
set(HIGHFIVE_UNIT_TESTS OFF CACHE BOOL "" FORCE)  # Don't build tests
set(HIGHFIVE_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build examples
set(HIGHFIVE_BUILD_DOCS OFF CACHE BOOL "" FORCE)  # Don't build docs
add_subdirectory(vendor/HighFive)

target_link_libraries(snirf PUBLIC HDF5::HDF5 HighFive)

# Compiler warnings (optional but recommended)
target_compile_options(snirf PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ============================================
# Installation
# ============================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS snirf
    EXPORT snirfTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets for find_package support
install(EXPORT snirfTargets
    FILE snirfTargets.cmake
    NAMESPACE snirf::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/snirf
)

# Generate config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/snirfConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/snirfConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/snirf
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/snirfConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/snirfConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/snirfConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/snirf
)

# ============================================
# Tests (optional)
# ============================================
if(SNIRF_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================
# Development Executable (optional)
# ============================================
if(SNIRF_BUILD_DEV_EXECUTABLE)
    add_executable(snirf_dev tests/snirf_test.cpp)
    target_link_libraries(snirf_dev PRIVATE snirf)
endif()

# ============================================
# Examples (optional)
# ============================================
if(SNIRF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()