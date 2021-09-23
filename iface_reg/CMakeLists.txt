find_package(Catch2 REQUIRED)

lib(
  NAME iface_reg
  STD cxx_std_20
  TEST_LIBS
    Catch2::Catch2
)
