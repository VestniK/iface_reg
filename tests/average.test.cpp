#include <numeric>

#include <tests/common_env.hpp>

class average : public plugin<average, stat_func_iface> {
public:
  static constexpr std::string_view name = "average";

  double calc(std::span<const double> sample) override {
    return std::accumulate(sample.begin(), sample.end(), 0.) / sample.size();
  }
};
