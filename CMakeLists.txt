project (lcab C)
set (lcab_VERSION_MAJOR 1)
set (lcab_VERSION_MINOR 1)

cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set Build Options
# set(CMAKE_BUILD_TYPE Debug)

include(CTest)

###############################################################################
# Generate "config.h" from "cmake/config.h.cmake"
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/src)
add_definitions(-DHAVE_CONFIG_H)
set(CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
