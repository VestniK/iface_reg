if (NOT WIN32)
  add_subdirectory(dynlib)
  set(DYNLIB_TGT integration_test_dynlib)
endif()

find_package(Catch2 REQUIRED)

cpp_unit(
  NAME iface_reg_integration
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
    catch2_main
    iface_reg
    ${DYNLIB_TGT}
  TEST_ARGS --rng-seed time
)
set_property(
  TARGET iface_reg_integration.test
  PROPERTY ENABLE_EXPORTS On
)
