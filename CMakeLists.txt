cmake_minimum_required(VERSION 3.14)

project(snirfcpp VERSION 1.0 LANGUAGES CXX)


# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(SNIRF_BUILD_TESTS "Build tests" OFF)
option(SNIRF_BUILD_EXAMPLES "Build examples" OFF)

# Define the static library
add_library(snirf STATIC
    "src/snirf.cpp"
    "include/snirf/snirf.h"
    "include/snirf/SNIRFFactory.h"
 "include/snirf/snirftype.h" "src/SNIRFFactory.cpp" "include/snirf/utils.h")

# Include directories
target_include_directories(snirf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# SNIRF files are HDF5-based, so you likely need this
find_package(HDF5 REQUIRED COMPONENTS CXX)
target_link_libraries(snirf PUBLIC HDF5::HDF5)

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
# Examples (optional)
# ============================================
if(SNIRF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()