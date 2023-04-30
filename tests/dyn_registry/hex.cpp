#include <charconv>

#include <tests/dyn_registry/env.hpp>

class hex_serializer
    : public dyn::plugin<hex_serializer, dyn::num_serializer_iface> {
public:
  constexpr static std::string_view name = "hex";

  std::string serialize(int val) override {
    std::array<char, 20> buf;
    const auto res =
        std::to_chars(buf.data(), buf.data() + buf.size(), val, 16);
    return std::string{buf.data(), res.ptr};
  }

  std::optional<int> deserialize(std::string_view str) override {
    int val;
    const char *first = str.data();
    const char *last = str.data() + str.size();

    if (std::from_chars(first, last, val, 16).ptr == last) {
      return val;
    }
    return std::nullopt;
  }
};
