#include <algorithm>
#include <filesystem>
#include <numeric>
#include <random>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <tests/ci_string.hpp>
#include <tests/common_env.hpp>
#include <tests/dlopen.hpp>
#include <tests/dyn_registry/env.hpp>
#include <tests/main.test.hpp>

using namespace Catch::literals;
using namespace ci::literals;

SCENARIO("app registry integration tests") {
  GIVEN("some random sample") {
    const std::vector<double> sample = [&] {
      std::mt19937 gen;
      std::normal_distribution<double> dist;
      constexpr size_t sample_sz = 5000;
      std::vector<double> res;
      res.reserve(sample_sz);
      std::generate_n(std::back_inserter(res), sample_sz,
                      [&] { return dist(gen); });
      return res;
    }();

    WHEN("plugin from the app itself is searched") {
      auto average_factory = registry.find_factory<stat_func_iface>("average");

      THEN("its factory is non null") { CHECK(average_factory != nullptr); }
      THEN("plugin found calculates sample average") {
        REQUIRE(average_factory != nullptr);
        auto average = average_factory();
        CHECK(average->calc(sample) == 0.0185213375_a);
      }
    }

#if !defined(WIN32)
    WHEN("plugin from dyn lib is searched") {
      auto median_factory = registry.find_factory<stat_func_iface>("median");

      THEN("its factory is non null") { CHECK(median_factory != nullptr); }
      THEN("plugin found calculates sample median") {
        REQUIRE(median_factory != nullptr);
        auto median = median_factory();
        CHECK(median->calc(sample) == 0.0217291609_a);
      }
    }
#endif

    WHEN("plugin from not yet loaded module searched") {
      auto deviation_factory =
          registry.find_factory<stat_func_iface>("standard_deviation");
      THEN("nothing is found") { CHECK(deviation_factory == nullptr); }
    }

    WHEN("module with plugin loaded") {
      auto plugin = dlopen(get_plugins_env().plugin_path);
      auto deviation_factory =
          registry.find_factory<stat_func_iface>("standard_deviation");
      THEN("factory for it's plugins are non null") {
        CHECK(deviation_factory != nullptr);
      }
      THEN("plugin found calculates standard deviation") {
        REQUIRE(deviation_factory != nullptr);
        auto deviation = deviation_factory();
        CHECK(deviation->calc(sample) == 1.0126213678_a);
      }
    }
  }
}

SCENARIO("dyn registry integration test") {
  GIVEN("some num") {
    const int val = 42;

    WHEN("serializer from app is searched") {
      auto app_factory =
          dyn::registry.find_factory<dyn::num_serializer_iface>("decimal");

      THEN("non-null factory returned") { CHECK(app_factory != nullptr); }
      THEN("created plugin serializes value to decimal strig") {
        REQUIRE(app_factory != nullptr);
        auto decimal = app_factory();
        CHECK(decimal->serialize(val) == "42");
      }
    }

    WHEN("serializer from registry dyn lib is searched") {
      auto dynreg_factory =
          dyn::registry.find_factory<dyn::num_serializer_iface>("hex");

      THEN("non-null factory returned") { CHECK(dynreg_factory != nullptr); }
      THEN("created plugin serializes value to hex strig") {
        REQUIRE(dynreg_factory != nullptr);
        auto hex = dynreg_factory();
        CHECK(hex->serialize(val) == "2A"_ci);
      }
    }
  }
}
