#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::array<std::uint8_t, 16> md5(std::string_view s);
std::uint64_t hash_s(std::string_view s, std::uint64_t ns);

struct Nodes {
  std::vector<std::string> ds;
  std::vector<std::uint64_t> ss;
  std::uint64_t ns;

  explicit Nodes(std::uint64_t ns);
  Nodes &add(std::string_view s);
  void done();
  std::pair<std::string, std::uint64_t> bs(std::string_view s) const;
};
