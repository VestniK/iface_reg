find_package(Catch2 REQUIRED)

cpp_unit(
  NAME catch2_main
  STD cxx_std_20
  LIBS
    Catch2::Catch2
)
