#pragma once

#include <memory>

#include <iface_reg/factory.hpp>

namespace iface_reg {

template <typename R> struct factory<std::unique_ptr<R>> {
  template <typename T, typename... A>
  requires std::is_constructible_v<T, A...> && std::derived_from<T, R>
  static std::unique_ptr<R> create(A &&...a) {
    return std::make_unique<T>(std::forward<A>(a)...);
  }
};

template <typename R> struct factory<std::shared_ptr<R>> {
  template <typename T, typename... A>
  requires std::is_constructible_v<T, A...> && std::derived_from<T, R>
  static std::shared_ptr<R> create(A &&...a) {
    return std::make_shared<T>(std::forward<A>(a)...);
  }
};

} // namespace iface_reg
