#include "consish.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <print>
#include <ranges>
#include <span>
#include <tuple>

static void md5_block(std::array<std::uint32_t, 4> &state,
                      std::span<const std::byte, 64> block) {
  static constexpr std::array<std::uint32_t, 64> k{
      0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
      0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
      0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
      0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
      0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
      0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
      0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
      0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
      0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
      0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
      0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
  };
  static constexpr std::array shifts{
      std::array{7, 12, 17, 22}, std::array{5, 9, 14, 20},
      std::array{4, 11, 16, 23}, std::array{6, 10, 15, 21}};

  std::array<std::uint32_t, 16> words;
  for (std::size_t i = 0; i < words.size(); ++i) {
    words[i] = std::to_integer<std::uint32_t>(block[4 * i]) |
               std::to_integer<std::uint32_t>(block[4 * i + 1]) << 8 |
               std::to_integer<std::uint32_t>(block[4 * i + 2]) << 16 |
               std::to_integer<std::uint32_t>(block[4 * i + 3]) << 24;
  }

  auto [a, b, c, d] = state;
  for (std::size_t i = 0; i < k.size(); ++i) {
    std::uint32_t f;
    std::size_t g;
    if (i < 16) {
      f = (b & c) | (~b & d);
      g = i;
    } else if (i < 32) {
      f = (b & d) | (c & ~d);
      g = (5 * i + 1) % 16;
    } else if (i < 48) {
      f = b ^ c ^ d;
      g = (3 * i + 5) % 16;
    } else {
      f = c ^ (b | ~d);
      g = 7 * i % 16;
    }

    std::uint32_t v = a + f + k[i] + words[g];
    a = d;
    d = c;
    c = b;
    b += std::rotl(v, shifts[i / 16][i % 4]);
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}

std::array<std::uint8_t, 16> md5(std::string_view s) {
  auto bytes = std::as_bytes(std::span{s.data(), s.size()});
  std::array<std::uint32_t, 4> state{0x67452301, 0xefcdab89, 0x98badcfe,
                                     0x10325476};
  while (bytes.size() >= 64) {
    md5_block(state, bytes.first<64>());
    bytes = bytes.subspan(64);
  }

  std::array<std::byte, 128> tail{};
  std::ranges::copy(bytes, tail.begin());
  tail[bytes.size()] = std::byte{0x80};
  std::size_t padded = bytes.size() < 56 ? 64 : 128;
  std::uint64_t bits = static_cast<std::uint64_t>(s.size()) * 8;
  for (std::size_t i = 0; i < 8; ++i)
    tail[padded - 8 + i] = static_cast<std::byte>(bits >> (8 * i));

  md5_block(state, std::span{tail}.first<64>());
  if (padded == 128)
    md5_block(state, std::span{tail}.subspan<64, 64>());

  std::array<std::uint8_t, 16> digest;
  for (std::size_t i = 0; i < state.size(); ++i)
    for (std::size_t j = 0; j < 4; ++j)
      digest[4 * i + j] = static_cast<std::uint8_t>(state[i] >> (8 * j));
  return digest;
}

std::uint64_t hash_s(std::string_view s, std::uint64_t ns) {
  assert(ns > 0 && "zero ring size");
  auto digest = md5(s);
  auto read_be = [](std::span<const std::uint8_t, 8> bytes) {
    return std::ranges::fold_left(bytes, std::uint64_t{0},
                                  [](auto a, auto b) { return (a << 8) | b; });
  };
  auto bytes = std::span{digest};
  return (read_be(bytes.first<8>()) ^ read_be(bytes.last<8>())) % ns;
}

Nodes::Nodes(std::uint64_t ns) : ns(ns) { assert(ns > 0 && "zero ring size"); }

Nodes &Nodes::add(std::string_view s) {
  auto hash = hash_s(s, ns);
  ds.emplace_back(s);
  try {
    ss.push_back(hash);
  } catch (...) {
    ds.pop_back();
    throw;
  }
  return *this;
}

void Nodes::done() {
  std::ranges::stable_sort(std::views::zip(ds, ss), {}, [](const auto &entry) {
    return std::get<1>(entry);
  });
}

std::pair<std::string, std::uint64_t> Nodes::bs(std::string_view s) const {
  assert(!ds.empty() && "no elements");
  auto hash = hash_s(s, ns);
  auto it = std::ranges::lower_bound(ss, hash);
  if (it == ss.end())
    it = ss.begin();
  auto i = static_cast<std::size_t>(it - ss.begin());
  return {ds[i], hash};
}

#ifndef TEST
int main() {
  Nodes nodes(65535);
  nodes.add("A").add("B").add("C").done();

  for (const auto &[name, hash] : std::views::zip(nodes.ds, nodes.ss))
    std::println("{}: {}", name, hash);
  std::println("ns: {}\n", nodes.ns);

  for (std::string_view s : {"1", "10", "100"}) {
    auto [name, hash] = nodes.bs(s);
    std::println("(\"{}\", {})", name, hash);
  }
}
#endif
