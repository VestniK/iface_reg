#include <iface_reg/ns.hpp>
#include <iface_reg/registry_buckets.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

irg::detail::registry_key make_test_key(std::string_view name,
                                        size_t iface_idx) noexcept {
  return {.plugin_name = name, .iface_idx = iface_idx};
}

irg::detail::registry_node make_test_node(std::string_view name,
                                          size_t iface_idx) noexcept {
  return {.key = make_test_key(name, iface_idx),
          .factory_func = nullptr,
          .next = nullptr};
}

} // namespace

SCENARIO("registyr_buckets colisions") {
  GIVEN("Contaier with two occupied buckets") {
    irg::detail::registry_buckets<32> buckets;
    auto node3 = make_test_node("qwe", 45);
    auto node8 = make_test_node("rty", 100500);
    buckets.link_at(3, node3);
    buckets.link_at(8, node8);

    THEN("nothing found at empty hash") {
      REQUIRE(buckets.find_at(7, node3.key) == nullptr);
    }

    THEN("node found by proper hash and key") {
      REQUIRE(buckets.find_at(3, node3.key) == &node3);
    }

    THEN("node not found by proper key wrong hash") {
      REQUIRE(buckets.find_at(8, node3.key) == nullptr);
      REQUIRE(buckets.find_at(3, node8.key) == nullptr);
    }
  }
}
