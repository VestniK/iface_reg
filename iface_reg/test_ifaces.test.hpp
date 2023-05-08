#pragma once

#include <memory>

struct int_iface {
  using factory_signature = std::unique_ptr<int_iface>();

  virtual int get_num() = 0;
  virtual ~int_iface() noexcept = default;
};

struct double_iface {
  using factory_signature = std::shared_ptr<double_iface>();

  virtual double get_num() = 0;
};

inline std::unique_ptr<int_iface> bad_int_factory() { return {}; };
inline std::shared_ptr<double_iface> bad_double_factory() { return {}; };
