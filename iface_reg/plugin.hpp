#pragma once

#include <iface_reg/concepts.hpp>
#include <iface_reg/record.hpp>

namespace iface_reg {

namespace detail {

template <plugin_interface Iface, typename Impl, auto &Registry>
requires registry_for<std::remove_cvref_t<decltype(Registry)>, Iface>
struct default_record {
  using registry_t = std::remove_cvref_t<decltype(Registry)>;

  default_record()
      : node{Registry.template make_node<Iface>(
            Impl::name, factory_function<Iface, Impl>)} {
    Registry.link(node);
  }

  ~default_record() noexcept {
    Registry.template unlink<Iface>(node.key.plugin_name);
  }

  registry_t::node node;
};

template <plugin_interface Iface, typename Impl, auto &Registry>
requires registry_for<std::remove_cvref_t<decltype(Registry)>, Iface>
inline default_record<Iface, Impl, Registry> default_plugin_record{};

} // namespace detail

template <typename Impl, plugin_interface Iface, auto &Registry,
          auto = &detail::default_plugin_record<Iface, Impl, Registry>>
requires registry_for<std::remove_cvref_t<decltype(Registry)>, Iface>
class plugin : public Iface {
  using interface = Iface;
  using implementation = Impl;
  static constexpr auto &get_regisrty() noexcept { return Registry; }

private:
  [[maybe_unused]] named_plugin_implementation<Iface> auto &
  consraint_checker() {
    return static_cast<Impl &>(*this);
  }
};

} // namespace iface_reg
