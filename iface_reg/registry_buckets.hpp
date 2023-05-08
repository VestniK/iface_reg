#pragma once

#include <array>

#include <iface_reg/registry_iterator.hpp>

namespace iface_reg::detail {

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

  registry_iterator begin() const noexcept {
    return registry_iterator{buckets_};
  }
  registry_sentinel end() const noexcept { return {}; }

  // Complexity O(BucketsCount)
  bool empty() const noexcept { return begin() == end(); }

private:
  std::array<registry_node *, BucketsCount> buckets_{};
};

} // namespace iface_reg::detail
