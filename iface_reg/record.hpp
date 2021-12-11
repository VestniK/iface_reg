#pragma once

#include <iface_reg/concepts.hpp>
#include <iface_reg/factory.hpp>

namespace iface_reg {

template <plugin_interface Iface, plugin_implementation<Iface> Impl,
          auto &Registry>
requires registry_for<std::remove_cvref_t<decltype(Registry)>, Iface>
struct record {
  using registry_t = std::remove_cvref_t<decltype(Registry)>;

  record(std::string_view name)
      : node{Registry.template make_node<Iface>(
            name, factory_function<Iface, Impl>)} {
    Registry.link(node);
  }

  ~record() noexcept { Registry.template unlink<Iface>(node.key.plugin_name); }

  registry_t::node node;
};

} // namespace iface_reg
