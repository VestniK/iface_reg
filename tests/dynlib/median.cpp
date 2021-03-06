#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

#include <tests/common_env.hpp>

#if defined(_WIN32)
#if defined(dynlib_EXPORTS)
#define DYNLIB_PUBLIC __declspec(dllexport)
#else
#define DYNLIB_PUBLIC __declspec(dllimport)
#endif
#elif __has_cpp_attribute(gnu::visibility)
#define DYNLIB_PUBLIC [[gnu::visibility("default")]]
#endif

class DYNLIB_PUBLIC median : public plugin<median, stat_func_iface> {
public:
  static constexpr std::string_view name = "median";

  double calc(std::span<const double> sample) override {
    if (sample.empty())
      return 0.;
    if (sample.size() == 1)
      return sample.front();
    std::vector<double> copy;
    copy.reserve(sample.size());
    std::ranges::copy(sample, std::back_inserter(copy));
    std::ranges::sort(copy);

    return (copy[copy.size() / 2] + copy[copy.size() / 2 + 1]) / 2;
  }
};
