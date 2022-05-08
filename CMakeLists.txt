cmake_minimum_required(VERSION 3.20)
project(iface_reg LANGUAGES CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(cmake/conan.cmake)
include(cmake/canonical_project.cmake)

conan_cmake_configure(
  REQUIRES
    catch2/2.13.9
    fmt/8.1.1
  GENERATORS cmake_find_package
)
conan_cmake_autodetect(settings)
conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conancenter
  SETTINGS ${settings}
)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
enable_testing()

add_subdirectory(catch2_main)
add_subdirectory(iface_reg)
add_subdirectory(tests)
