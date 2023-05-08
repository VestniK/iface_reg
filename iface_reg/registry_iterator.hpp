#pragma once

#include <compare>
#include <span>
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

struct registry_sentinel {};
class registry_iterator {
public:
  using value_type = const registry_node;
  using difference_type = std::ptrdiff_t;

  registry_iterator() noexcept = default;
  registry_iterator(std::span<registry_node *const> roots) noexcept
      : roots_tail_{roots} {
    find_next_root();
  }

  value_type &operator*() const noexcept { return *cur_; }
  registry_iterator &operator++() noexcept {
    if (!cur_)
      return *this;
    if (cur_->next)
      cur_ = cur_->next;
    else {
      find_next_root();
    }
    return *this;
  }
  registry_iterator operator++(int) noexcept {
    auto res = *this;
    ++(*this);
    return res;
  }

  bool operator==(const registry_iterator &rhs) const noexcept {
    return roots_tail_.data() == rhs.roots_tail_.data() &&
           roots_tail_.size() == rhs.roots_tail_.size();
  }

private:
  void find_next_root() noexcept {
    cur_ = nullptr;
    while (!roots_tail_.empty() && !cur_) {
      cur_ = roots_tail_.front();
      roots_tail_ = roots_tail_.subspan(1);
    }
  }

  friend bool operator==(registry_sentinel,
                         const registry_iterator &rhs) noexcept {
    return rhs.cur_ == nullptr;
  }
  friend bool operator==(const registry_iterator &lhs,
                         registry_sentinel) noexcept {
    return lhs.cur_ == nullptr;
  }

private:
  const registry_node *cur_ = nullptr;
  std::span<value_type *const> roots_tail_;
};

struct registry_iface_items {
  registry_iterator begin() const noexcept { return pos; }
  registry_sentinel end() const noexcept { return {}; }

  registry_iterator pos;
};

} // namespace iface_reg::detail
