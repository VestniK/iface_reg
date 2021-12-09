find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

cpp_unit(
  NAME iface_reg
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
    fmt::fmt
    catch2_main
  TEST_ARGS --rng-seed time
)
