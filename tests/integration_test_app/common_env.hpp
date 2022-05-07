#pragma once

#include <memory>
#include <span>

#include <iface_reg/basic_registry.hpp>
#include <iface_reg/ns.hpp>
#include <iface_reg/plugin.hpp>
#include <iface_reg/std_factory.hpp>

struct stat_func_iface {
  using factory_signature = std::unique_ptr<stat_func_iface>();

  virtual ~stat_func_iface() noexcept = default;
  virtual double calc(std::span<const double> sample) = 0;
};

#if defined(_WIN32)
#if defined(iface_reg_integration_EXPORTS) ||                                  \
    defined(iface_reg_integration_test_EXPORTS)
#define COMON_ENV_PUBLIC __declspec(dllexport)
#else
#define COMON_ENV_PUBLIC __declspec(dllimport)
#endif
#elif __has_cpp_attribute(gnu::visibility)
#define COMON_ENV_PUBLIC [[gnu::visibility("default")]]
#endif

COMON_ENV_PUBLIC
extern irg::registry<stat_func_iface> registry;

template <typename Impl, typename Iface>
using plugin = irg::plugin<Impl, Iface, registry>;
