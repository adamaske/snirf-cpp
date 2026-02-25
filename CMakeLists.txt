cmake_minimum_required(VERSION 3.14)

project(snirfcpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Standalone build ? 
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SNIRF_STANDALONE ON)
else()
    set(SNIRF_STANDALONE OFF)
endif()

option(SNIRF_BUILD_TESTS "Build tests" OFF)
option(SNIRF_BUILD_EXAMPLES "Build examples" OFF)
option(SNIRF_BUILD_DEV_EXECUTABLE "Build development executable" ${SNIRF_STANDALONE})

add_library(snirfcpp STATIC
    "src/SNIRFLoader.cpp"
)

target_include_directories(snirfcpp
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

target_link_libraries(snirfcpp PUBLIC HDF5::HDF5 HighFive)

# Compiler warnings (optional but recommended)
target_compile_options(snirfcpp PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ============================================
# Installation
# ============================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS snirfcpp
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
    target_link_libraries(snirf_dev PRIVATE snirfcpp)
endif()

#if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
#    message(STATUS "Building in standalone mode.")
#    add_executable(snirf-cpp tests/snirf_test.cpp)
#    target_link_libraries(snirf-cpp PRIVATE snirf)
#else()
#    message(STATUS "Building as part of a larger project.")
#endif()

# ============================================
# Examples (optional)
# ============================================
if(SNIRF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

