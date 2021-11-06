cmake_minimum_required(VERSION 3.20)
project(iface_reg LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(cmake/conan.cmake)
include(cmake/ProjectStruct.cmake)

conan_cmake_configure(
  REQUIRES
    catch2/3.0.0@catchorg/stable
    fmt/7.1.3
  GENERATORS cmake_find_package
)
conan_cmake_autodetect(settings)
conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  REMOTE conan-center
  SETTINGS ${settings}
)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
enable_testing()
add_subdirectory(iface_reg)
