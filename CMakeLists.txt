cmake_minimum_required(VERSION 3.20)
project(iface_reg LANGUAGES CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(cmake/canonical_project.cmake)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
enable_testing()

add_subdirectory(iface_reg)
add_subdirectory(tests)
