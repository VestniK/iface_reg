#pragma once

#include <iface_reg/concepts.hpp>

namespace iface_reg {

template <typename FactoryResult> struct factory;

template <typename T, typename FactorySignature> struct make_factory_func;

template <typename T, typename R, typename... A>
struct make_factory_func<T, R(A...)> {
  using factory_signature = R(A...);

  constexpr static factory_signature *value =
      &factory<R>::template create<T, A...>;
};

template <plugin_interface I, plugin_implementation<I> P>
constexpr typename I::factory_signature *factory_function =
    make_factory_func<P, typename I::factory_signature>::value;

} // namespace iface_reg
