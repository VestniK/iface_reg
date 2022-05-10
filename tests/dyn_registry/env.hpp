#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <iface_reg/basic_registry.hpp>
#include <iface_reg/ns.hpp>
#include <iface_reg/plugin.hpp>
#include <iface_reg/std_factory.hpp>

#if defined(_WIN32)
#if defined(dyn_registry_EXPORTS)
#define DYN_REGISTRY_PUBLIC __declspec(dllexport)
#else
#define DYN_REGISTRY_PUBLIC __declspec(dllimport)
#endif
#elif __has_cpp_attribute(gnu::visibility)
#define DYN_REGISTRY_PUBLIC [[gnu::visibility("default")]]
#endif

namespace dyn {

struct num_serializer_iface {
  using factory_signature = std::unique_ptr<num_serializer_iface>();

  virtual ~num_serializer_iface() noexcept = default;
  virtual std::string serialize(int) = 0;
  virtual std::optional<int> deserialize(std::string_view str) = 0;
};

DYN_REGISTRY_PUBLIC
extern irg::registry<num_serializer_iface> registry;

template <typename Impl, typename Iface>
using plugin = irg::plugin<Impl, Iface, registry>;

} // namespace dyn
