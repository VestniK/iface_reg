#pragma once

#include <cstddef>
#include <utility>

namespace iface_reg::detail {

template <typename... Ts> struct type_list {
  template <typename T, size_t... Idx>
  static constexpr size_t find(std::index_sequence<Idx...>) noexcept {
    constexpr size_t end = sizeof...(Ts);
    return (end - ... - (std::is_same_v<T, Ts> ? end - Idx : 0));
  }
};

template <typename T, typename... Ts> constexpr size_t find() noexcept {
  return type_list<Ts...>::template find<T>(
      std::make_index_sequence<sizeof...(Ts)>());
}

} // namespace iface_reg::detail
