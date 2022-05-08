#include <tests/dlopen.hpp>

#include <windows.h>

void dlcloser::operator()(void* handle) const noexcept {
  if (handle)
    FreeLibrary(handle);
}
dyn_lib dlopen(const std::filesystem::path &path) {
  return dyn_lib{LoadLibrary(path.c_str())};
}
