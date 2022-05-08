#include <algorithm>
#include <numeric>
#include <iterator>
#include <ranges>
#include <vector>

#include <tests/common_env.hpp>

#if defined(_WIN32)
#if defined(dynplugin_EXPORTS)
#define DYNPLUGIN_PUBLIC __declspec(dllexport)
#else
#define DYNPLUGIN_PUBLIC __declspec(dllimport)
#endif
#elif __has_cpp_attribute(gnu::visibility)
#define DYNPLUGIN_PUBLIC [[gnu::visibility("default")]]
#endif

class DYNPLUGIN_PUBLIC standard_deviation : public plugin<standard_deviation, stat_func_iface> {
public:
  static constexpr std::string_view name = "standard_deviation";

  double calc(std::span<const double> sample) override {
    if (sample.size() < 2)
      return 0;
    struct {size_t count = 0; double mean = 0; double m2 = 0;} start;
    const auto agg = std::accumulate(sample.begin(), sample.end(), start, [](auto agg, double val) {
        ++agg.count;
        const double delta = val - agg.mean;
        agg.mean += delta/agg.count;
        agg.m2 += delta*(val - agg.mean);
        return agg;
    });
    return agg.m2/agg.count;
  }
};
