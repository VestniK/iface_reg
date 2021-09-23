#pragma once

#include <concepts>
#include <type_traits>

namespace iface_reg {

namespace detail {

template <typename T> struct is_signature : std::false_type {};

template <typename R, typename... A>
struct is_signature<R(A...)> : std::true_type {};

template <typename T> constexpr bool is_signature_v = is_signature<T>::value;

template <typename T, typename FacorySignature>
requires is_signature_v<FacorySignature>
struct is_factory_constructible : std::false_type {
};

template <typename T, typename R, typename... A>
struct is_factory_constructible<T, R(A...)>
    : std::integral_constant<bool, std::is_constructible_v<T, A...>> {};

template <typename T, typename FacorySignature>
requires is_signature_v<FacorySignature>
constexpr bool is_factory_constructible_v =
    is_factory_constructible<T, FacorySignature>::value;

} // namespace detail

template <typename T>
concept plugin_interface = std::is_abstract_v<T> &&
    detail::is_signature_v<typename T::factory_signature>;

template <typename T, typename Interface>
concept plugin_implementation =
    plugin_interface<Interface> && std::derived_from<T, Interface> &&
    detail::is_factory_constructible_v<T,
                                       typename Interface::factory_signature>;

} // namespace iface_reg
