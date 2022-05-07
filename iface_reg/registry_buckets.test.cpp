#include <iface_reg/ns.hpp>
#include <iface_reg/registry_buckets.hpp>

#include <catch2/catch.hpp>

#include <fmt/format.h>

namespace Catch {
template <> struct StringMaker<irg::detail::registry_node> {
  static std::string convert(const irg::detail::registry_node &value) {
    return fmt::format("{}[@{}] -> {:x}", value.key.plugin_name,
                       value.key.iface_idx,
                       reinterpret_cast<uintptr_t>(&value));
  }
};
} // namespace Catch

namespace {

template <typename Arr> auto random_elem(const Arr &arr) {
  using namespace Catch::Generators;
  return map([&arr](size_t i) { return arr[i]; },
             random(size_t{0}, std::size(arr) - 1));
}

template <size_t BucketsCount>
class found_at_bucket : public Catch::MatcherBase<irg::detail::registry_node> {
public:
  found_at_bucket(const irg::detail::registry_buckets<BucketsCount> &buckets,
                  size_t bucket) noexcept
      : buckets_{&buckets}, bucket_num_{bucket} {}

  bool match(const irg::detail::registry_node &node) const override {
    return buckets_->find_at(bucket_num_, node.key) == &node;
  }

  std::string describe() const override {
    return fmt::format("found node at bucket {}", bucket_num_);
  }

private:
  const irg::detail::registry_buckets<BucketsCount> *buckets_;
  size_t bucket_num_;
};

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

    WHEN("Another node added with same hash") {
      auto collision = make_test_node("asd", 45);
      buckets.link_at(8, collision);

      THEN("previous nodes are searchable") {
        REQUIRE(buckets.find_at(3, node3.key) == &node3);
        REQUIRE(buckets.find_at(8, node8.key) == &node8);
      }

      THEN("new node is found by proper hash") {
        REQUIRE(buckets.find_at(8, collision.key) == &collision);
      }

      THEN("new node is is not found by wrong hash") {
        REQUIRE(buckets.find_at(3, collision.key) == nullptr);
      }

      THEN("removing old node at hash with collision succeeds") {
        REQUIRE(buckets.unlink_at(8, node8.key) == true);
      }

      THEN("removing new node at hash with collision succeeds") {
        REQUIRE(buckets.unlink_at(8, collision.key) == true);
      }

      THEN("old node with hash collision is not found after removal") {
        buckets.unlink_at(8, node8.key);
        REQUIRE(buckets.find_at(8, node3.key) == nullptr);
      }

      THEN("new node with hash collision is found after old node removal") {
        buckets.unlink_at(8, node8.key);
        REQUIRE(buckets.find_at(8, collision.key) == &collision);
      }

      THEN("new node with hash collision is not found after removal") {
        buckets.unlink_at(8, collision.key);
        REQUIRE(buckets.find_at(8, collision.key) == nullptr);
      }

      THEN("old node with hash collision is found after new node removal") {
        buckets.unlink_at(8, collision.key);
        REQUIRE(buckets.find_at(8, node8.key) == &node8);
      }
    }
  }
}

constexpr size_t buckets_count = 32;
constexpr size_t buckets_to_fill[] = { 0, buckets_count - 1, buckets_count / 3,
                                      2 * buckets_count / 3 };
constexpr static std::string_view all_names[] = {
    "qwe",  "rty", "asd", "fgh",  "zxc", "vbn",
    "uiop", "123", "345", "wsad", "abc", "def" };

SCENARIO("buckets link/unlink/find fuzz tests", "[fuzz]") {
  const auto sample_sz = GENERATE(take(3, random(4u, 18u)));

  const auto bckt_nums =
      GENERATE_COPY(take(3, chunk(sample_sz, random_elem(buckets_to_fill))));
  const auto ifaces = GENERATE_COPY(take(2, chunk(sample_sz, random(0u, 2u))));
  const auto names =
      GENERATE_COPY(take(5, chunk(sample_sz, random_elem(all_names))));

  std::vector<irg::detail::registry_node> nodes;
  nodes.reserve(sample_sz);
  for (size_t i = 0; i < sample_sz; ++i)
    nodes.push_back(make_test_node(names[i], ifaces[i]));

  GIVEN("Empty registry_buckets") {
    irg::detail::registry_buckets<buckets_count> buckets;

    THEN("linking node fails only if node with equivalnt key was added") {
      for (size_t i = 0; i < sample_sz; ++i) {
        const auto existing_node = buckets.find_at(bckt_nums[i], nodes[i].key);
        CHECK(existing_node != &nodes[i]);
        REQUIRE(buckets.link_at(bckt_nums[i], nodes[i]) == !existing_node);
      }
    }

    WHEN("It's filled with test nodes") {
      std::vector<std::pair<size_t, irg::detail::registry_node *>> inserted,
          duplicates;
      for (size_t i = 0; i < sample_sz; ++i) {
        if (buckets.link_at(bckt_nums[i], nodes[i]))
          inserted.push_back({bckt_nums[i], &nodes[i]});
        else
          duplicates.push_back({bckt_nums[i], &nodes[i]});
      }

      THEN("all inserted nodes are searchable") {
        for (const auto &[bckt, node] : inserted)
          REQUIRE_THAT(*node, found_at_bucket(buckets, bckt));
      }

      THEN("all duplicates are not searchable") {
        for (const auto &[bckt, node] : duplicates)
          REQUIRE_THAT(*node, !found_at_bucket(buckets, bckt));
      }

      THEN("node found by duplicate key always have key equal to requested") {
        for (const auto &[bckt, node] : duplicates) {
          const auto *fres = buckets.find_at(bckt, node->key);
          CHECK(fres != nullptr);
          REQUIRE(fres->key == node->key);
        }
      }

      THEN("insert same node twice always fails") {
        for (const auto &[bckt, node] : inserted)
          REQUIRE(!buckets.link_at(bckt, *node));
      }

      THEN("unlink inserted node always succeeds") {
        for (const auto &[bckt, node] : inserted)
          REQUIRE(buckets.unlink_at(bckt, node->key));
      }

      THEN("unlink inserted node with same kye allows to link duplicate") {
        for (const auto &[bckt, node] : duplicates) {
          buckets.unlink_at(bckt, node->key);
          REQUIRE(buckets.link_at(bckt, *node));
        }
      }

      AND_WHEN("Conflicting nodes for all duplicates are unlinked and "
               "duplicates are linked") {
        for (const auto &[bckt, node] : duplicates)
          buckets.unlink_at(bckt, node->key);
        std::erase_if(duplicates, [&](auto item) {
          return !buckets.link_at(item.first, *item.second);
        });

        THEN("all newly inserted elements are searchable") {
          for (const auto &[bckt, node] : duplicates)
            REQUIRE_THAT(*node, found_at_bucket(buckets, bckt));
        }
      }
    }
  }
}
