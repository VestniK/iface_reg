if (NOT WIN32)
  add_subdirectory(dynlib)
  set(DYNLIB_TGT dynlib)
endif()

add_subdirectory(dynplugin)

find_package(Catch2 REQUIRED)

cpp_unit(
  NAME iface_reg_integration
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
    iface_reg
    ${DYNLIB_TGT}
    ${CMAKE_DL_LIBS}
  TEST_ARGS --rng-seed time -p $<TARGET_FILE:dynplugin>
)
set_property(
  TARGET iface_reg_integration.test
  PROPERTY ENABLE_EXPORTS On
)
if (WIN32)
  target_sources(iface_reg_integration.test PRIVATE win/dlopen.cpp)
  set_property(SOURCES win/dlopen.cpp PROPERTY COMPILE_DEFINITIONS UNICODE)
else()
  target_sources(iface_reg_integration.test PRIVATE unix/dlopen.cpp)
endif()
