#include <memory>

#include <iface_reg/basic_registry.hpp>
#include <iface_reg/concepts.hpp>
#include <iface_reg/ns.hpp>

// Signature traits test
static_assert(!irg::detail::is_signature_v<int>);
static_assert(irg::detail::is_signature_v<int()>);
static_assert(irg::detail::is_signature_v<int(double, char *)>);
static_assert(irg::detail::is_signature_v<void(double, char *)>);

// plugin_interface tests
struct A {};
struct B {
  virtual int foo() { return 42; }
};
struct C {
  int foo() { return 42; }
};
static_assert(!irg::plugin_interface<A>, "interface must be abstract class");
static_assert(!irg::plugin_interface<B>, "interface must be abstract class");
static_assert(!irg::plugin_interface<C>, "interface must be abstract class");

// TODO: investigate how to check absense of non static data members
// struct D {
//  int x = 0;
//};
// struct E : D {
//  virtual void foo() = 0;
//};
// static_assert(!irg::plugin_interface<D>, "interface must have no data
// members"); static_assert(!irg::plugin_interface<E>, "interface must have no
// data members");

struct F {
  virtual void foo() = 0;
};
static_assert(!irg::plugin_interface<F>,
              "plugin interface must specify factory signature");

struct iface {
  using factory_signature = std::unique_ptr<iface>(int);
  virtual void foo() = 0;
};
static_assert(irg::plugin_interface<iface>);

// plugin_implementation tests
static_assert(!irg::plugin_implementation<A, iface>,
              "implementation must derive from interface");
struct subclass : iface {
  void foo() override {}
};
static_assert(!irg::plugin_implementation<subclass, iface>,
              "implementation must be constructible with factory arguments");

struct impl : iface {
  impl(int) {}
  void foo() override {}
};
static_assert(irg::plugin_implementation<impl, iface>,
              "implementation must be constructible with factory arguments");

struct iface_2 {
  using factory_signature = std::unique_ptr<iface_2>(int);
  virtual void bar() = 0;
};
struct iface_3 {
  using factory_signature = std::unique_ptr<iface_3>(int);
  virtual void baz() = 0;
};

static_assert(
    irg::registry_for<irg::registry<iface>, iface>,
    "registry satisfies registry_for concept for it's the only parameter");

static_assert(irg::registry_for<irg::registry<iface, iface_2>, iface>,
              "registry satisfies registry_for concept for it's first template "
              "parameter");

static_assert(irg::registry_for<irg::registry<iface, iface_2>, iface_2>,
              "registry satisfies registry_for concept for it's second "
              "template parameter");

static_assert(
    !irg::registry_for<irg::registry<iface, iface_3>, iface_2>,
    "registry satisfies registry_for concept for it's arguments only");
