#include <numeric>
#include <random>

#include <catch2/catch.hpp>

#include <tests/common_env/common_env.hpp>

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
      std::normal_distribution dist;
      constexpr size_t sample_sz = 5000;
      std::vector<double> res;
      res.reserve(sample_sz);
      std::generate_n(std::back_inserter(res), sample_sz,
                      [&] { return dist(gen); });
      return res;
    }();

    WHEN("plugin from the app itself is searched") {
      auto average_factory = registry.find_factory<stat_func_iface>("average");

      THEN("its factory is non null") { REQUIRE(average_factory != nullptr); }
      THEN("plugin calculate sample average") {
        auto average = average_factory();
        REQUIRE(average->calc(sample) == 0.0185213375_a);
      }
    }

    WHEN("plugin from dyn lib is searched") {
      auto median_factory = registry.find_factory<stat_func_iface>("median");

      THEN("its factory is non null") { REQUIRE(median_factory != nullptr); }
      THEN("plugin calculate sample median") {
        auto median = median_factory();
        REQUIRE(median->calc(sample) == 0.0217291609_a);
      }
    }
  }
}
