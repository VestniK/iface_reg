find_package(Catch2 REQUIRED)

cpp_unit(
  TYPE SHARED
  NAME integration_test_dynlib
  STD cxx_std_20
)
