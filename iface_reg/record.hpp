#pragma once

#include <iface_reg/concepts.hpp>
#include <iface_reg/factory.hpp>

namespace iface_reg {

template <plugin_interface Iface, plugin_implementation<Iface> Impl,
          auto &Registry>
requires registry_for<decltype(Registry), Iface>
struct record {
  record(std::string_view name)
      : node{Registry.template make_node<Iface>(
            name, factory_function<Iface, Impl>)} {
    Registry.link(node);
  }

  ~record() noexcept { Registry.template unlink<Iface>(node.key.plugin_name); }

  typename decltype(Registry)::node node;
};

template <plugin_interface Iface,
          named_plugin_implementation<Iface> Impl, auto &Registry>
requires registry_for<decltype(Registry), Iface>
struct default_record : record<Iface, Impl, Registry> {
  default_record() : record<Iface, Impl, Registry>{Impl::name} {}
};

template <plugin_interface Iface,
          named_plugin_implementation<Iface> Impl, auto &Registry>
requires registry_for<decltype(Registry), Iface>
inline default_record<Iface, Impl, Registry> default_plugin_record{};

}
