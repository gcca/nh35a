#include "nh35a.h"

int main() {
  struct heapq_t *heapq = heapq_create(50);

  for (size_t i = 30; i > 0; i--) {
    heapq_push(heapq, i);
  }
  heapq_push(heapq, 0);

  printf("print:\n");
  heapq_print(heapq);

  printf("\n\ntraverse ld:\n");
  heapq_traverse_ld(heapq);

  printf("\n\npop all:\n");
  for (size_t i = 0; i < 30; i++) {
    printf("%zu ", heapq_pop(heapq));
  }
  printf("\n");

  heapq_release(heapq);

  return 0;
}
