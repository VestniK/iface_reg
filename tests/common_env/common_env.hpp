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

extern irg::registry<stat_func_iface> registry;

template <typename Impl, typename Iface>
using plugin = irg::plugin<Impl, Iface, registry>;
