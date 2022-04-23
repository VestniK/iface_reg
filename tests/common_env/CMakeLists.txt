find_package(Catch2 REQUIRED)

if (WIN32)
  set(TYPE_FLAGS TYPE SHARED)
endif()

cpp_unit(
  ${TYPE_FLAGS}
  NAME integration_test_env
  LIBS iface_reg
  STD cxx_std_20
)
