#include <charconv>

#include <tests/dyn_registry/env.hpp>

class dec_serializer
    : public dyn::plugin<dec_serializer, dyn::num_serializer_iface> {
public:
  constexpr static std::string_view name = "decimal";

  std::string serialize(int val) override {
    std::array<char, 20> buf;
    const auto res =
        std::to_chars(buf.data(), buf.data() + buf.size(), val, 10);
    return std::string{buf.data(), res.ptr};
  }

  std::optional<int> deserialize(std::string_view str) override {
    int val;
    if (std::from_chars(str.begin(), str.end(), val, 10).ptr == str.end()) {
      return val;
    }
    return std::nullopt;
  }
};
