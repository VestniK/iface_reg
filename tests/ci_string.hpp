#pragma once

#include <cctype>
#include <string>

namespace ci {

struct char_traits : std::char_traits<char> {
  static char to_upper(char ch) noexcept {
    return std::toupper((unsigned char)ch);
  }
  static bool eq(char c1, char c2) noexcept {
    return to_upper(c1) == to_upper(c2);
  }
  static bool lt(char c1, char c2) noexcept {
    return to_upper(c1) < to_upper(c2);
  }
  static int compare(const char *s1, const char *s2, std::size_t n) noexcept {
    while (n-- != 0) {
      if (to_upper(*s1) < to_upper(*s2))
        return -1;
      if (to_upper(*s1) > to_upper(*s2))
        return 1;
      ++s1;
      ++s2;
    }
    return 0;
  }
  static const char *find(const char *s, std::size_t n, char a) noexcept {
    auto const ua(to_upper(a));
    while (n-- != 0) {
      if (to_upper(*s) == ua)
        return s;
      s++;
    }
    return nullptr;
  }
};

using string = std::basic_string<char, char_traits, std::allocator<char>>;
using string_view = std::basic_string_view<char, char_traits>;
namespace literals {
constexpr string_view operator""_ci(const char *str, size_t len) noexcept {
  return {str, len};
}
} // namespace literals

bool operator==(const std::string &l, string_view r) noexcept {
  return string_view{l.data(), l.size()} == r;
}
bool operator==(std::string_view l, string_view r) noexcept {
  return string_view{l.data(), l.size()} == r;
}

bool operator==(string_view l, const std::string &r) noexcept {
  return l == string_view{r.data(), r.size()};
}
bool operator==(string_view l, std::string_view r) noexcept {
  return l == string_view{r.data(), r.size()};
}

} // namespace ci
