#pragma once

#include <iface_reg/concepts.hpp>
#include <iface_reg/record.hpp>

namespace iface_reg {

template <typename Impl, plugin_interface Iface, auto &Registry,
          auto = &default_plugin_record<Iface, Impl, Registry>>
requires plugin_implementation<Impl, Iface> &&
    registry_for<decltype(Registry), Iface>
class plugin : public Iface {
  using interface = Iface;
  using implementation = Impl;
  static constexpr auto &get_regisrty() noexcept { return Registry; }
};

}
