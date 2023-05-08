#pragma once

#include <ranges>
#include <string_view>

#include <iface_reg/concepts.hpp>
#include <iface_reg/registry_buckets.hpp>
#include <iface_reg/registry_iterator.hpp>
#include <iface_reg/type_list.hpp>

namespace iface_reg {

template <size_t BucketsCount, template <typename> typename Hash,
          plugin_interface... Ifaces>
class basic_registry : private detail::registry_buckets<BucketsCount> {
private:
  using base_t = detail::registry_buckets<BucketsCount>;

public:
  using node = detail::registry_node;

public:
  template <plugin_interface I>
    requires std::disjunction_v<std::is_same<I, Ifaces>...>
  static constexpr detail::registry_node
  make_node(std::string_view name,
            typename I::factory_signature *factory) noexcept {
    constexpr size_t iface_idx = detail::find<I, Ifaces...>();
    static_assert(iface_idx < sizeof...(Ifaces));
    return {.key = {.plugin_name = name, .iface_idx = iface_idx},
            .factory_func =
                reinterpret_cast<detail::registry_node::typerased_factory *>(
                    factory),
            .next = nullptr};
  }

  template <plugin_interface I>
    requires std::disjunction_v<std::is_same<I, Ifaces>...>
  typename I::factory_signature *
  find_factory(std::string_view name) const noexcept {
    constexpr size_t iface_idx = detail::find<I, Ifaces...>();
    static_assert(iface_idx < sizeof...(Ifaces));
    detail::registry_key key{.plugin_name = name, .iface_idx = iface_idx};

    auto *node = base_t::find_at(hash(key), key);
    return node ? reinterpret_cast<typename I::factory_signature *>(
                      node->factory_func)
                : nullptr;
  }

  template <plugin_interface I>
    requires std::disjunction_v<std::is_same<I, Ifaces>...>
  auto items() const noexcept {
    constexpr size_t iface_idx = detail::find<I, Ifaces...>();
    static_assert(iface_idx < sizeof...(Ifaces));
    return static_cast<const detail::registry_buckets<BucketsCount> &>(*this) |
           std::views::filter([](const detail::registry_node &node) {
             return node.key.iface_idx == iface_idx;
           }) |
           std::views::transform([](const detail::registry_node &node) {
             return std::pair{node.key.plugin_name,
                              reinterpret_cast<typename I::factory_signature *>(
                                  node.factory_func)};
           });
  }

  bool link(detail::registry_node &node) noexcept {
    return base_t::link_at(hash(node.key), node);
  }

  template <plugin_interface I>
    requires std::disjunction_v<std::is_same<I, Ifaces>...> bool
  unlink(std::string_view name) noexcept {
    constexpr size_t idx = detail::find<I, Ifaces...>();
    static_assert(idx < sizeof...(Ifaces));
    detail::registry_key key{.plugin_name = name, .iface_idx = idx};
    return base_t::unlink_at(hash(key), key);
  }

private:
  size_t hash(detail::registry_key key) const noexcept {
    size_t res = Hash<std::string_view>{}(key.plugin_name);
    res ^= key.iface_idx + 0x9e3779b9 + (res << 6) + (res >> 2);
    return res;
  }
};

template <plugin_interface... Ifaces>
using registry = basic_registry<512, std::hash, Ifaces...>;

} // namespace iface_reg
