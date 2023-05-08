#include <algorithm>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fmt/format.h>

#include <iface_reg/basic_registry.hpp>
#include <iface_reg/ns.hpp>

#include <iface_reg/test_ifaces.test.hpp>

static_assert(std::forward_iterator<irg::detail::registry_iterator>);
static_assert(std::sentinel_for<irg::detail::registry_sentinel,
                                irg::detail::registry_iterator>);
static_assert(
    std::ranges::forward_range<irg::detail::registry_buckets<100500>>);
static_assert(std::is_same_v<
              std::ranges::range_value_t<irg::detail::registry_buckets<100500>>,
              irg::detail::registry_node>);

using Catch::Matchers::UnorderedRangeEquals;
using namespace std::literals;

namespace {

template <irg::plugin_interface Iface>
auto elements_are(
    std::vector<
        std::pair<std::string_view, typename Iface::factory_signature *>>
        elems) {
  return UnorderedRangeEquals(std::move(elems));
}

struct is_empty_matcher : Catch::Matchers::MatcherGenericBase {
  template <std::ranges::range OtherRange>
  bool match(OtherRange &&range) const {
    return range.begin() == range.end();
  }

  std::string describe() const override { return "Is empty"; }
};

is_empty_matcher is_empty() noexcept { return is_empty_matcher{}; }

} // namespace

namespace Catch {

template <typename R, typename... A>
struct StringMaker<std::pair<std::string_view, R (*)(A...)>> {
  using pair_t = std::pair<std::string_view, R (*)(A...)>;
  static std::string convert(const pair_t &kv) {
    return fmt::format("{{'{}': 0x{:x}}}", kv.first,
                       reinterpret_cast<uintptr_t>(kv.second));
  }
};

} // namespace Catch

SCENARIO("registry::iterate") {
  GIVEN("some empty registry") {
    irg::registry<int_iface, double_iface> reg;
    static_assert(std::ranges::forward_range<decltype(reg.items<int_iface>())>);
    static_assert(std::is_same_v<
                  std::ranges::range_value_t<decltype(reg.items<int_iface>())>,
                  std::pair<std::string_view, int_iface::factory_signature *>>);
    static_assert(
        std::is_same_v<
            std::ranges::range_value_t<decltype(reg.items<double_iface>())>,
            std::pair<std::string_view, double_iface::factory_signature *>>);

    THEN("items for some iface returns empty range") {
      CHECK_THAT(reg.items<double_iface>(), is_empty());
    }

    WHEN("factory for first iface is registered") {
      auto first_node = reg.make_node<int_iface>("first", bad_int_factory);
      reg.link(first_node);

      THEN("items iterate over added elem when proper iface is requested") {
        CHECK_THAT(reg.items<int_iface>(),
                   elements_are<int_iface>({{"first", bad_int_factory}}));
      }

      THEN("items returns empty range for other ifaces") {
        CHECK_THAT(reg.items<double_iface>(), is_empty());
      }
    }
  }
}
