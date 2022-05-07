find_package(Catch2 REQUIRED)

if (NOT WIN32)
  set(SHARED_APP_IFACE_IMPL integration_test_dynlib)
endif()

cpp_unit(
  NAME iface_reg_integration
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
    catch2_main
    iface_reg
    ${SHARED_APP_IFACE_IMPL}
  TEST_ARGS --rng-seed time
)
set_property(
  TARGET iface_reg_integration.test
  PROPERTY ENABLE_EXPORTS On
)