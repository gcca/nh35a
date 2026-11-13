#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char **ds;
  uint64_t *ss;
  size_t len;
  size_t capacity;
  uint64_t ns;
} Nodes;

typedef struct {
  const char *name;
  uint64_t hash;
} Lookup;

void md5(const void *data, size_t len, unsigned char digest[16]);
uint64_t hash_s(const char *s, uint64_t ns);
Nodes nodes_init(uint64_t ns);
void nodes_deinit(Nodes *n);
void nodes_add(Nodes *n, const char *s);
void nodes_done(Nodes *n);
Lookup nodes_bs(const Nodes *n, const char *s);

#ifdef __cplusplus
}
#endif
