#include <numeric>
#include <random>

#include <catch2/catch.hpp>

#include "common_env.test.hpp"

using namespace Catch::literals;

class average : public plugin<average, stat_func_iface> {
public:
  static constexpr std::string_view name = "average";

  double calc(std::span<double> sample) override {
    return std::accumulate(sample.begin(), sample.end(), 0.) / sample.size();
  }
};

SCENARIO("integration tests") {
  GIVEN("some random sample") {
    std::mt19937 gen;
    std::normal_distribution dist;
    constexpr size_t sample_sz = 5000;
    std::vector<double> sample;
    sample.reserve(sample_sz);
    std::generate_n(std::back_inserter(sample), sample_sz,
                    [&] { return dist(gen); });

    WHEN("average plugin is searched") {
      auto average_factory = registry.find_factory<stat_func_iface>("average");

      THEN("its factory is non null") { REQUIRE(average_factory != nullptr); }
      THEN("plugin calculate sample average") {
        auto average = average_factory();
        REQUIRE(average->calc(sample) == 0.0185213375_a);
      }
    }
  }
}