#pragma once

#include <array>
#include <compare>
#include <string_view>

namespace iface_reg::detail {

struct registry_key {
  constexpr std::strong_ordering
  operator<=>(const registry_key &) const noexcept = default;

  std::string_view plugin_name;
  size_t iface_idx;
};

struct registry_node {
  using typerased_factory = void();

  registry_key key;
  typerased_factory *factory_func;
  registry_node *next;
};

template <size_t BucketsCount> class registry_buckets {
public:
  bool link_at(size_t hash, registry_node &node) noexcept {
    auto **pos = &buckets_[hash % buckets_.size()];
    while (*pos) {
      if ((*pos)->key == node.key)
        return false;
      pos = &(*pos)->next;
    }
    *pos = &node;
    return true;
  }

  bool unlink_at(size_t hash, registry_key key) noexcept {
    auto **pos = &buckets_[hash % buckets_.size()];
    while (*pos && (*pos)->key != key)
      pos = &(*pos)->next;
    if (!*pos)
      return false;
    *pos = (*pos)->next;
    return true;
  }

  registry_node *find_at(size_t hash, registry_key key) const noexcept {
    auto *node = buckets_[hash % buckets_.size()];
    while (node && node->key != key)
      node = node->next;

    return node;
  }

private:
  std::array<registry_node *, BucketsCount> buckets_{};
};

} // namespace iface_reg::detail
