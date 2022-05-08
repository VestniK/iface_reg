#include <tests/dlopen.hpp>

#include <dlfcn.h>

void dlcloser::operator()(void *handle) const noexcept {
  if (handle)
    dlclose(handle);
}
dyn_lib dlopen(const std::filesystem::path &path) {
  return dyn_lib{dlopen(path.c_str(), RTLD_LAZY)};
}
