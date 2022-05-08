#pragma once

#include <filesystem>
#include <memory>

struct dlcloser {
  void operator()(void *handle) const noexcept;
};
using dyn_lib = std::unique_ptr<void, dlcloser>;

dyn_lib dlopen(const std::filesystem::path &paht);
