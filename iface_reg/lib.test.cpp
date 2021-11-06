#include <iface_reg/basic_registry.hpp>
#include <iface_reg/concepts.hpp>
#include <iface_reg/factory.hpp>
#include <iface_reg/ns.hpp>
#include <memory>

template <typename P, typename I>
concept named_plugin_implementation = irg::plugin_implementation<P, I> &&
    requires() {
  { P::name } -> std::convertible_to<std::string_view>;
};

template <irg::plugin_interface Iface, irg::plugin_implementation<Iface> Impl,
          auto &Registry>
requires irg::registry_for<decltype(Registry), Iface>
struct record {
  record(std::string_view name)
      : node{Registry.template make_node<Iface>(
            name, irg::factory_function<Iface, Impl>)} {
    Registry.link(node);
  }

  ~record() { Registry.template unlink<Iface>(node.key.plugin_name); }

  typename decltype(Registry)::node node;
};

template <irg::plugin_interface Iface, named_plugin_implementation<Iface> Impl,
          auto &Registry>
requires irg::registry_for<decltype(Registry), Iface>
struct default_record : record<Iface, Impl, Registry> {
  default_record() : record<Iface, Impl, Registry>{Impl::name} {}
};

template <irg::plugin_interface Iface, named_plugin_implementation<Iface> Impl,
          auto &Registry>
requires irg::registry_for<decltype(Registry), Iface>
inline default_record<Iface, Impl, Registry> default_plugin_record{};

template <typename Impl, irg::plugin_interface Iface, auto &Registry,
          auto = &default_plugin_record<Iface, Impl, Registry>>
requires irg::plugin_implementation<Impl, Iface> &&
    irg::registry_for<decltype(Registry), Iface>
class plugin : public Iface {
  using interface = Iface;
  using implementation = Impl;
  static constexpr auto &get_regisrty() noexcept { return Registry; }
};

int fn() { return 0; }
