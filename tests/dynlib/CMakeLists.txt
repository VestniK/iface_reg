cpp_unit(
  TYPE SHARED
  NAME integration_test_dynlib
  LIBS iface_reg
  STD cxx_std_20
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_options(integration_test_dynlib PUBLIC
    "LINKER:--no-as-needed"
  )
endif()
