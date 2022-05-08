#include <filesystem>
#include <numeric>
#include <random>

#include <catch2/catch.hpp>

#include <tests/common_env.hpp>
#include <tests/dlopen.hpp>
#include <tests/main.test.hpp>

using namespace Catch::literals;

class average : public plugin<average, stat_func_iface> {
public:
  static constexpr std::string_view name = "average";

  double calc(std::span<const double> sample) override {
    return std::accumulate(sample.begin(), sample.end(), 0.) / sample.size();
  }
};

SCENARIO("integration tests") {
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
        auto average = average_factory();
        REQUIRE(average_factory != nullptr);
        CHECK(average->calc(sample) == 0.0185213375_a);
      }
    }

#if !defined(WIN32)
    WHEN("plugin from dyn lib is searched") {
      auto median_factory = registry.find_factory<stat_func_iface>("median");

      THEN("its factory is non null") { CHECK(median_factory != nullptr); }
      THEN("plugin found calculates sample median") {
        auto median = median_factory();
        REQUIRE(median_factory != nullptr);
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
      THEN("factory fror it's plugins are non null") {
        CHECK(deviation_factory != nullptr);
      }
      THEN("plugin found calculates standard deviation") {
        auto deviation = deviation_factory();
        REQUIRE(deviation_factory != nullptr);
        CHECK(deviation->calc(sample) == 1.0126213678_a);
      }
    }
  }
}
