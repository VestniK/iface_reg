find_package(Catch2 REQUIRED)

cpp_unit(
  NAME iface_reg_integration
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
    catch2_main
    iface_reg
    integration_test_env
    integration_test_dynlib
  TEST_ARGS --rng-seed time
)
