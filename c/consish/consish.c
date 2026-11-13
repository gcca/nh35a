#include "consish.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void md5_block(uint32_t state[4], const unsigned char block[64]) {
  static constexpr uint32_t k[64] = {
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
  static constexpr unsigned shifts[4][4] = {
      {7, 12, 17, 22}, {5, 9, 14, 20}, {4, 11, 16, 23}, {6, 10, 15, 21}};

  uint32_t words[16];
  for (size_t i = 0; i < 16; ++i) {
    words[i] = (uint32_t)block[4 * i] | (uint32_t)block[4 * i + 1] << 8 |
               (uint32_t)block[4 * i + 2] << 16 |
               (uint32_t)block[4 * i + 3] << 24;
  }

  uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
  for (size_t i = 0; i < 64; ++i) {
    uint32_t f;
    size_t g;
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

    uint32_t v = a + f + k[i] + words[g];
    unsigned shift = shifts[i / 16][i % 4];
    a = d;
    d = c;
    c = b;
    b += (v << shift) | (v >> (32 - shift));
  }
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}

void md5(const void *data, size_t len, unsigned char digest[16]) {
  const unsigned char *bytes = data;
  uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
  size_t offset = 0;
  while (len - offset >= 64) {
    md5_block(state, bytes + offset);
    offset += 64;
  }

  unsigned char tail[128] = {0};
  size_t remaining = len - offset;
  memcpy(tail, bytes + offset, remaining);
  tail[remaining] = 0x80;
  size_t padded = remaining < 56 ? 64 : 128;
  uint64_t bits = (uint64_t)len * 8;
  for (size_t i = 0; i < 8; ++i)
    tail[padded - 8 + i] = (unsigned char)(bits >> (8 * i));

  md5_block(state, tail);
  if (padded == 128)
    md5_block(state, tail + 64);

  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      digest[4 * i + j] = (unsigned char)(state[i] >> (8 * j));
}

uint64_t hash_s(const char *s, uint64_t ns) {
  assert(ns > 0 && "zero ring size");
  unsigned char digest[16];
  md5(s, strlen(s), digest);

  uint64_t dl = 0, dh = 0;
  for (size_t i = 0; i < 8; ++i) {
    dl = (dl << 8) | digest[i];
    dh = (dh << 8) | digest[8 + i];
  }
  return (dh ^ dl) % ns;
}

Nodes nodes_init(uint64_t ns) {
  assert(ns > 0 && "zero ring size");
  return (Nodes){.ns = ns};
}

void nodes_deinit(Nodes *n) {
  for (size_t i = 0; i < n->len; ++i)
    free(n->ds[i]);
  free(n->ds);
  free(n->ss);
  *n = (Nodes){0};
}

void nodes_add(Nodes *n, const char *s) {
  if (n->len == n->capacity) {
    if (n->capacity > SIZE_MAX / sizeof(*n->ds) / 2 ||
        n->capacity > SIZE_MAX / sizeof(*n->ss) / 2) {
      fputs("too many nodes\n", stderr);
      exit(EXIT_FAILURE);
    }
    size_t capacity = n->capacity ? n->capacity * 2 : 4;
    char **ds = realloc(n->ds, capacity * sizeof(*ds));
    if (ds == nullptr) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }
    n->ds = ds;
    uint64_t *ss = realloc(n->ss, capacity * sizeof(*ss));
    if (ss == nullptr) {
      perror("realloc");
      exit(EXIT_FAILURE);
    }
    n->ss = ss;
    n->capacity = capacity;
  }

  char *name = strdup(s);
  if (name == nullptr) {
    perror("strdup");
    exit(EXIT_FAILURE);
  }
  n->ds[n->len] = name;
  n->ss[n->len] = hash_s(s, n->ns);
  ++n->len;
}

void nodes_done(Nodes *n) {
  for (size_t i = 1; i < n->len; ++i) {
    char *name = n->ds[i];
    uint64_t hash = n->ss[i];
    size_t j = i;
    while (j > 0 && n->ss[j - 1] > hash) {
      n->ds[j] = n->ds[j - 1];
      n->ss[j] = n->ss[j - 1];
      --j;
    }
    n->ds[j] = name;
    n->ss[j] = hash;
  }
}

Lookup nodes_bs(const Nodes *n, const char *s) {
  assert(n->len > 0 && "no elements");
  uint64_t sh = hash_s(s, n->ns);
  size_t lo = 0, hi = n->len;
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (n->ss[mid] < sh)
      lo = mid + 1;
    else
      hi = mid;
  }
  size_t i = lo == n->len ? 0 : lo;
  return (Lookup){.name = n->ds[i], .hash = sh};
}

#ifndef TEST
int main(void) {
  constexpr uint64_t ring_size = 65535;
  Nodes ns = nodes_init(ring_size);
  nodes_add(&ns, "A");
  nodes_add(&ns, "B");
  nodes_add(&ns, "C");
  nodes_done(&ns);

  for (size_t i = 0; i < ns.len; ++i)
    printf("%s: %" PRIu64 "\n", ns.ds[i], ns.ss[i]);
  printf("ns: %" PRIu64 "\n\n", ns.ns);

  const char *keys[] = {"1", "10", "100"};
  for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
    Lookup result = nodes_bs(&ns, keys[i]);
    printf("(\"%s\", %" PRIu64 ")\n", result.name, result.hash);
  }
  nodes_deinit(&ns);
  return 0;
}
#endif
