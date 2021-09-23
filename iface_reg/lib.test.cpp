#include <array>
#include <functional>
#include <string_view>
#include <type_traits>

#include <iface_reg/concepts.hpp>
#include <iface_reg/ns.hpp>
#include <iface_reg/registry_buckets.hpp>
#include <iface_reg/std_factory.hpp>
#include <iface_reg/type_list.hpp>

namespace iface_reg {

template <size_t BucketsCount, template <typename> typename Hash,
          plugin_interface... Ifaces>
class basic_registry : private detail::registry_buckets<BucketsCount> {
private:
  using base_t = detail::registry_buckets<BucketsCount>;

public:
  template <plugin_interface I>
  static constexpr detail::registry_node
  make_node(std::string_view name,
            typename I::factory_signature *factory) noexcept {
    constexpr size_t iface_idx = detail::find<I, Ifaces...>();
    static_assert(iface_idx < sizeof...(Ifaces));
    return {.key = {.plugin_name = name, .iface_idx = iface_idx},
            .factory_func =
                reinterpret_cast<detail::registry_node::typerased_factory *>(
                    factory),
            .next = nullptr};
  }

  template <plugin_interface I>
  typename I::factory_signature *
  find_factory(std::string_view name) const noexcept {
    constexpr size_t iface_idx = detail::find<I, Ifaces...>();
    static_assert(iface_idx < sizeof...(Ifaces));
    detail::registry_key key{.plugin_name = name, .iface_idx = iface_idx};

    auto *node = base_t::find_at(hash(key), key);
    return node ? reinterpret_cast<typename I::factory_signature *>(
                      node->factory_func)
                : nullptr;
  }

  bool link(detail::registry_node &node) noexcept {
    return base_t::link_at(hash(node.key), node);
  }

  template <plugin_interface I> bool unlink(std::string_view name) noexcept {
    constexpr size_t idx = detail::find<I, Ifaces...>();
    static_assert(idx < sizeof...(Ifaces));
    detail::registry_key key{.plugin_name = name, .iface_idx = idx};
    return base_t::unlink_at(hash(key), key);
  }

private:
  size_t hash(detail::registry_key key) const noexcept {
    size_t res = Hash<std::string_view>{}(key.plugin_name);
    res ^= key.iface_idx + 0x9e3779b9 + (res << 6) + (res >> 2);
    return res;
  }
};

template <plugin_interface... Ifaces>
using registry = basic_registry<512, std::hash, Ifaces...>;

} // namespace iface_reg

// TESTS

#include <catch2/catch_test_macros.hpp>

struct int_iface {
  using factory_signature = std::unique_ptr<int_iface>();

  virtual int get_num() = 0;
};

struct double_iface {
  using factory_signature = std::shared_ptr<double_iface>();

  virtual double get_num() = 0;
};

SCENARIO("registry::link") {
  GIVEN("Empty registry") {
    irg::registry<int_iface, double_iface> reg;

    auto bad_int_factory = []() { return std::unique_ptr<int_iface>{}; };
    auto bad_double_factory = []() { return std::shared_ptr<double_iface>{}; };

    THEN("link any bucket succeeds") {
      auto bad = reg.make_node<int_iface>("bad_bucket", bad_int_factory);
      REQUIRE(reg.link(bad) == true);
    }

    WHEN("Some factory added") {
      auto existing = reg.make_node<int_iface>("bad_bucket", bad_int_factory);
      reg.link(existing);

      THEN("link fails on name and iface colision") {
        auto bad = reg.make_node<int_iface>("bad_bucket", bad_int_factory);
        REQUIRE(reg.link(bad) == false);
      }

      THEN("link succeeds on same name for diferent iface") {
        auto bad =
            reg.make_node<double_iface>("bad_bucket", bad_double_factory);
        REQUIRE(reg.link(bad) == true);
      }

      THEN("link succeeds on same iface with diferent name") {
        auto bad =
            reg.make_node<int_iface>("another_bad_bucket", bad_int_factory);
        REQUIRE(reg.link(bad) == true);
      }
    }
  }
}

SCENARIO("registry::unlink") {
  GIVEN("Empty registry") {
    irg::registry<int_iface, double_iface> reg;
    THEN("any unlink fails") { REQUIRE(reg.unlink<int_iface>("qwe") == false); }

    WHEN("Some factories added") {
      auto bad_int_factory = []() { return std::unique_ptr<int_iface>{}; };
      auto bad_double_factory = []() {
        return std::shared_ptr<double_iface>{};
      };

      auto int_node = reg.make_node<int_iface>("bad int", bad_int_factory);
      auto double_node =
          reg.make_node<double_iface>("bad double", bad_double_factory);

      reg.link(int_node);
      reg.link(double_node);

      THEN("removing non existing node fails") {
        REQUIRE(reg.unlink<double_iface>("qwe") == false);
      }

      THEN("removing existing node succeeds") {
        REQUIRE(reg.unlink<double_iface>("bad double") == true);
      }

      THEN("removing existing name for wrong iface failes") {
        REQUIRE(reg.unlink<int_iface>("bad double") == false);
      }
    }
  }
}

SCENARIO("registry::find_factory") {
  GIVEN("Empty registry") {
    irg::registry<int_iface, double_iface> reg;

    THEN("any search returns null") {
      REQUIRE(reg.find_factory<double_iface>("qwe") == nullptr);
    }

    WHEN("Some factories are added") {
      auto bad_int_factory = []() { return std::unique_ptr<int_iface>{}; };
      auto bad_double_factory = []() {
        return std::shared_ptr<double_iface>{};
      };

      auto int_node = reg.make_node<int_iface>("bad int", bad_int_factory);
      auto double_node =
          reg.make_node<double_iface>("bad double", bad_double_factory);

      reg.link(int_node);
      reg.link(double_node);

      THEN("search for existing factory returns registered function") {
        REQUIRE(reg.find_factory<int_iface>("bad int") == bad_int_factory);
      }

      THEN("search for existing name for wrong iface returns null") {
        REQUIRE(reg.find_factory<int_iface>("bad double") == nullptr);
      }

      THEN("search for non existing name returns null") {
        REQUIRE(reg.find_factory<double_iface>("qwe") == nullptr);
      }

      AND_WHEN("Factories with the same name for multiple interfaces added") {
        auto very_bad = reg.make_node<int_iface>("bad double", bad_int_factory);
        reg.link(very_bad);

        THEN("search for first iface returns first factory") {
          REQUIRE(reg.find_factory<int_iface>("bad double") == bad_int_factory);
        }

        THEN("search for second iface returns second factory") {
          REQUIRE(reg.find_factory<double_iface>("bad double") ==
                  bad_double_factory);
        }
      }
    }
  }
}
