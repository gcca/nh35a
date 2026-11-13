#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "consish.h"

using testing::_;
using testing::ElementsAre;
using testing::IsEmpty;
using testing::Pair;
using testing::StrEq;

static std::string md5_hex(std::string_view s) {
  unsigned char digest[16];
  md5(s.data(), s.size(), digest);
  constexpr char digits[] = "0123456789abcdef";
  std::string hex(32, '0');
  for (size_t i = 0; i < 16; ++i) {
    hex[2 * i] = digits[digest[i] >> 4];
    hex[2 * i + 1] = digits[digest[i] & 15];
  }
  return hex;
}

TEST(Md5Test, MatchesReferenceVectors) {
  constexpr struct {
    const char *s;
    const char *digest;
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
    size_t len;
    const char *digest;
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

TEST(HashTest, MatchesMainValues) {
  constexpr struct {
    const char *s;
    uint64_t hash;
  } values[] = {
      {"A", 14321}, {"B", 8630},   {"C", 35588},
      {"1", 21135}, {"10", 41300}, {"100", 19522},
  };
  for (const auto &[s, hash] : values) {
    SCOPED_TRACE(s);
    EXPECT_EQ(hash_s(s, 65535), hash);
  }
}

class NodesTest : public testing::Test {
protected:
  Nodes nodes = nodes_init(65535);

  void TearDown() override { nodes_deinit(&nodes); }

  void reset(uint64_t ns) {
    nodes_deinit(&nodes);
    nodes = nodes_init(ns);
  }

  void add(std::initializer_list<const char *> names) {
    for (const char *name : names)
      nodes_add(&nodes, name);
  }

  std::vector<std::pair<std::string, uint64_t>> entries() const {
    std::vector<std::pair<std::string, uint64_t>> result;
    for (size_t i = 0; i < nodes.len; ++i)
      result.emplace_back(nodes.ds[i], nodes.ss[i]);
    return result;
  }

  std::pair<std::string, uint64_t> lookup(const char *s) const {
    Lookup result = nodes_bs(&nodes, s);
    return {result.name, result.hash};
  }
};

TEST_F(NodesTest, SortsEmptyRing) {
  nodes_done(&nodes);
  EXPECT_THAT(entries(), IsEmpty());
}

TEST_F(NodesTest, SortsHashesAndKeepsNamesAligned) {
  add({"A", "B", "C"});
  nodes_done(&nodes);
  EXPECT_THAT(entries(),
              ElementsAre(Pair("B", 8630), Pair("A", 14321), Pair("C", 35588)));
}

TEST_F(NodesTest, MatchesLookupsIncludingWraparoundAndExactMatches) {
  add({"A", "B", "C"});
  nodes_done(&nodes);
  constexpr struct {
    const char *s;
    const char *name;
    uint64_t hash;
  } values[] = {
      {"1", "C", 21135}, {"10", "B", 41300}, {"100", "C", 19522},
      {"", "B", 5970},   {"A", "A", 14321},  {"B", "B", 8630},
      {"C", "C", 35588},
  };
  for (const auto &[s, name, hash] : values) {
    SCOPED_TRACE(s);
    EXPECT_THAT(lookup(s), Pair(name, hash));
  }
}

TEST_F(NodesTest, SupportsUnsignedRingSizes) {
  reset(UINT64_MAX);
  add({"A", "B", "C"});
  nodes_done(&nodes);
  EXPECT_THAT(entries(), ElementsAre(Pair("C", _), Pair("A", _), Pair("B", _)));
  EXPECT_EQ(hash_s("B", nodes.ns), UINT64_C(18185419767222700475));
  for (const char *name : {"A", "B", "C"}) {
    SCOPED_TRACE(name);
    EXPECT_THAT(lookup(name).first, StrEq(name));
  }
  for (const char *s : {"1", "10", "100"}) {
    SCOPED_TRACE(s);
    EXPECT_THAT(lookup(s).first, StrEq("B"));
  }
}

TEST_F(NodesTest, FindsSingleNodeForEveryKey) {
  add({"A"});
  nodes_done(&nodes);
  for (const char *s : {"", "A", "1", "10", "100"}) {
    SCOPED_TRACE(s);
    EXPECT_THAT(lookup(s).first, StrEq("A"));
  }
}

TEST_F(NodesTest, PreservesInsertionOrderForCollisions) {
  reset(1);
  add({"C", "A", "B"});
  nodes_done(&nodes);
  EXPECT_THAT(entries(), ElementsAre(Pair("C", 0), Pair("A", 0), Pair("B", 0)));
  EXPECT_THAT(lookup("10"), Pair("C", 0));
}

TEST_F(NodesTest, GrowsAndOwnsAddedNames) {
  reset(UINT64_MAX);
  for (size_t i = 0; i < 32; ++i) {
    std::string name = "node-" + std::to_string(i);
    nodes_add(&nodes, name.c_str());
  }
  ASSERT_EQ(nodes.len, 32u);
  for (size_t i = 0; i < nodes.len; ++i) {
    SCOPED_TRACE(i);
    EXPECT_THAT(nodes.ds[i], StrEq("node-" + std::to_string(i)));
  }
  nodes_done(&nodes);
  for (size_t i = 0; i < nodes.len; ++i) {
    std::string name = "node-" + std::to_string(i);
    SCOPED_TRACE(name);
    EXPECT_THAT(lookup(name.c_str()).first, StrEq(name));
  }
}

TEST(HashDeathTest, RejectsZeroRingSize) {
  EXPECT_DEATH(hash_s("A", 0), "zero ring size");
}

TEST(NodesDeathTest, RejectsZeroRingSize) {
  EXPECT_DEATH(nodes_init(0), "zero ring size");
}

TEST(NodesDeathTest, RejectsEmptyLookup) {
  EXPECT_DEATH(
      {
        Nodes nodes = nodes_init(65535);
        nodes_bs(&nodes, "1");
      },
      "no elements");
}
