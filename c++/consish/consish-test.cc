#include <cstddef>
#include <initializer_list>
#include <limits>
#include <string>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "consish.hpp"

using testing::ElementsAre;
using testing::IsEmpty;
using testing::Pair;
using testing::StrEq;

static std::string md5_hex(std::string_view s) {
  constexpr char digits[] = "0123456789abcdef";
  std::string hex;
  hex.reserve(32);
  for (auto byte : md5(s)) {
    hex += digits[byte >> 4];
    hex += digits[byte & 15];
  }
  return hex;
}

TEST(Md5Test, MatchesReferenceVectors) {
  constexpr struct {
    std::string_view s;
    std::string_view digest;
  } values[] = {
      {"", "d41d8cd98f00b204e9800998ecf8427e"},
      {"a", "0cc175b9c0f1b6a831c399e269772661"},
      {"abc", "900150983cd24fb0d6963f7d28e17f72"},
      {"message digest", "f96b697d7cb7938d525a2f31aaf161d0"},
      {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
      {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
       "d174ab98d277d9f5a5611c2c9f419d9f"},
      {"1234567890123456789012345678901234567890"
       "1234567890123456789012345678901234567890",
       "57edf4a22be3c955ac49da2e2107b67a"},
  };
  for (const auto &[s, digest] : values) {
    SCOPED_TRACE(s);
    EXPECT_THAT(md5_hex(s), StrEq(digest));
  }
}

TEST(Md5Test, HandlesPaddingBoundaries) {
  constexpr struct {
    std::size_t len;
    std::string_view digest;
  } values[] = {
      {55, "ef1772b6dff9a122358552954ad0df65"},
      {56, "3b0c8ac703f828b04c6c197006d17218"},
      {63, "b06521f39153d618550606be297466d5"},
      {64, "014842d480b571495a4a0363793f7367"},
      {65, "c743a45e0d2e6a95cb859adae0248435"},
  };
  for (const auto &[len, digest] : values) {
    SCOPED_TRACE(len);
    EXPECT_THAT(md5_hex(std::string(len, 'a')), StrEq(digest));
  }
}

TEST(Md5Test, HonorsStringViewLength) {
  constexpr std::string_view s = "abc ignored";
  EXPECT_THAT(md5_hex(s.substr(0, 3)),
              StrEq("900150983cd24fb0d6963f7d28e17f72"));
  EXPECT_THAT(md5_hex({}), StrEq("d41d8cd98f00b204e9800998ecf8427e"));
}

TEST(HashTest, MatchesMainValues) {
  constexpr struct {
    std::string_view s;
    std::uint64_t hash;
  } values[] = {
      {"A", 14321}, {"B", 8630},   {"C", 35588},
      {"1", 21135}, {"10", 41300}, {"100", 19522},
  };
  for (const auto &[s, hash] : values) {
    SCOPED_TRACE(s);
    EXPECT_EQ(hash_s(s, 65535), hash);
  }
}

TEST(HashTest, HonorsStringViewLength) {
  constexpr std::string_view s = "A ignored";
  EXPECT_EQ(hash_s(s.substr(0, 1), 65535), 14321u);
}

class NodesTest : public testing::Test {
protected:
  Nodes nodes{65535};

  void add(std::initializer_list<std::string_view> names) {
    for (auto name : names)
      nodes.add(name);
  }
};

TEST_F(NodesTest, SortsEmptyRing) {
  nodes.done();
  EXPECT_THAT(nodes.ds, IsEmpty());
  EXPECT_THAT(nodes.ss, IsEmpty());
}

TEST_F(NodesTest, SortsHashesAndKeepsNamesAligned) {
  nodes.add("A").add("B").add("C").done();
  EXPECT_THAT(nodes.ds, ElementsAre("B", "A", "C"));
  EXPECT_THAT(nodes.ss, ElementsAre(8630, 14321, 35588));
}

TEST_F(NodesTest, MatchesLookupsIncludingWraparoundAndExactMatches) {
  add({"A", "B", "C"});
  nodes.done();
  constexpr struct {
    std::string_view s;
    std::string_view name;
    std::uint64_t hash;
  } values[] = {
      {"1", "C", 21135}, {"10", "B", 41300}, {"100", "C", 19522},
      {"", "B", 5970},   {"A", "A", 14321},  {"B", "B", 8630},
      {"C", "C", 35588},
  };
  for (const auto &[s, name, hash] : values) {
    SCOPED_TRACE(s);
    EXPECT_THAT(nodes.bs(s), Pair(name, hash));
  }
}

TEST_F(NodesTest, SupportsUnsignedRingSizes) {
  nodes = Nodes(std::numeric_limits<std::uint64_t>::max());
  add({"A", "B", "C"});
  nodes.done();
  EXPECT_THAT(nodes.ds, ElementsAre("C", "A", "B"));
  EXPECT_EQ(hash_s("B", nodes.ns), UINT64_C(18185419767222700475));
  for (std::string_view name : {"A", "B", "C"}) {
    SCOPED_TRACE(name);
    EXPECT_THAT(nodes.bs(name).first, StrEq(name));
  }
  for (std::string_view s : {"1", "10", "100"}) {
    SCOPED_TRACE(s);
    EXPECT_THAT(nodes.bs(s).first, StrEq("B"));
  }
}

TEST_F(NodesTest, FindsSingleNodeForEveryKey) {
  add({"A"});
  nodes.done();
  for (std::string_view s : {"", "A", "1", "10", "100"}) {
    SCOPED_TRACE(s);
    EXPECT_THAT(nodes.bs(s).first, StrEq("A"));
  }
}

TEST_F(NodesTest, PreservesInsertionOrderForCollisions) {
  nodes = Nodes(1);
  add({"C", "A", "B"});
  nodes.done();
  EXPECT_THAT(nodes.ds, ElementsAre("C", "A", "B"));
  EXPECT_THAT(nodes.ss, ElementsAre(0, 0, 0));
  EXPECT_THAT(nodes.bs("10"), Pair("C", 0));
}

TEST_F(NodesTest, GrowsAndOwnsAddedNames) {
  nodes = Nodes(std::numeric_limits<std::uint64_t>::max());
  for (std::size_t i = 0; i < 32; ++i) {
    std::string name = "node-" + std::to_string(i);
    nodes.add(name);
  }
  ASSERT_EQ(nodes.ds.size(), 32u);
  ASSERT_EQ(nodes.ss.size(), nodes.ds.size());
  for (std::size_t i = 0; i < nodes.ds.size(); ++i) {
    SCOPED_TRACE(i);
    EXPECT_THAT(nodes.ds[i], StrEq("node-" + std::to_string(i)));
  }
  nodes.done();
  for (std::size_t i = 0; i < nodes.ds.size(); ++i) {
    std::string name = "node-" + std::to_string(i);
    SCOPED_TRACE(name);
    EXPECT_THAT(nodes.bs(name).first, StrEq(name));
  }
}

TEST_F(NodesTest, HonorsStringViewLengths) {
  constexpr std::string_view s = "A ignored";
  nodes.add(s.substr(0, 1)).done();
  EXPECT_THAT(nodes.ds, ElementsAre("A"));
  EXPECT_THAT(nodes.ss, ElementsAre(14321));
  EXPECT_THAT(nodes.bs(s.substr(0, 1)), Pair("A", 14321));
}

TEST(HashDeathTest, RejectsZeroRingSize) {
  EXPECT_DEATH(hash_s("A", 0), "zero ring size");
}

TEST(NodesDeathTest, RejectsZeroRingSize) {
  EXPECT_DEATH((void)Nodes(0), "zero ring size");
}

TEST(NodesDeathTest, RejectsEmptyLookup) {
  EXPECT_DEATH(
      {
        Nodes nodes(65535);
        nodes.bs("1");
      },
      "no elements");
}
