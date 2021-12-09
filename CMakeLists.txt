cmake_minimum_required(VERSION 3.20)
project(iface_reg LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(cmake/conan.cmake)
include(cmake/canonical_project.cmake)

conan_cmake_configure(
  REQUIRES
    catch2/2.13.7
    fmt/7.1.3
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
add_subdirectory(iface_reg)
