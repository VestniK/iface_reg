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

struct registry_node;

class node_ptr {
public:
  struct tagged_t {};
  static constexpr tagged_t tagged = {};

  node_ptr(registry_node *ptr = nullptr) noexcept
      : val_{reinterpret_cast<uintptr_t>(ptr)} {}
  node_ptr(registry_node *ptr, tagged_t) noexcept : val_{to_tagged(ptr)} {}

  bool is_tagged() const noexcept { return val_ & 1; }
  void tag(bool val = true) noexcept { val_ |= 1; };
  void untag(bool val = true) noexcept { val_ &= ~1; };

  explicit operator bool() const noexcept { return to_ptr(val_) != nullptr; }

  registry_node *operator->() const noexcept { return to_ptr(val_); }
  registry_node *get() const noexcept { return to_ptr(val_); }

private:
  static uintptr_t to_tagged(registry_node *ptr) noexcept {
    return reinterpret_cast<uintptr_t>(ptr) | 1;
  }

  static registry_node *to_ptr(uintptr_t val) noexcept {
    return reinterpret_cast<registry_node *>(val & ~1);
  }

private:
  uintptr_t val_;
};

struct registry_node {
  using typerased_factory = void();

  registry_key key;
  typerased_factory *factory_func;
  node_ptr next;
};
static_assert(alignof(registry_node) > 1);

template <size_t BucketsCount> class registry_buckets {
public:
  bool link_at(size_t hash, registry_node &node) noexcept {
    auto *pos = &buckets_[hash % buckets_.size()];
    if (!*pos) {
      *pos = &node;
      node.next = std::exchange(head_, &node);
      node.next.tag();
      return true;
    }

    while (!pos->is_tagged()) {
      if ((*pos)->key == node.key)
        return false;
      pos = &(*pos)->next;
    }
    node.next = std::exchange(*pos, &node);
    return true;
  }

  bool unlink_at(size_t hash, registry_key key) noexcept {
    auto *pos = &buckets_[hash % buckets_.size()];
    while (*pos && !pos->is_tagged() && (*pos)->key != key)
      pos = &(*pos)->next;
    if (!*pos)
      return false;
    *pos = (*pos)->next;
    return true;
  }

  registry_node *find_at(size_t hash, registry_key key) const noexcept {
    auto node = buckets_[hash % buckets_.size()];
    while (node && !node.is_tagged() && node->key != key)
      node = node->next;

    return node.is_tagged() ? nullptr : node.get();
  }

private:
  std::array<node_ptr, BucketsCount> buckets_{};
  node_ptr head_;
};

} // namespace iface_reg::detail
