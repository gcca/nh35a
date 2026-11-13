#include "types.h"

typedef struct NHStack_ NHStack;
typedef struct NHStackNode_ NHStackNode;

struct NHStack_
{
  struct NHStackNode_
  {
    nhptr data;
    NHStackNode *next;
  } *root;
};

[[nodiscard]] NHStack *nh_stack_new ();
void nh_stack_free (NHStack *stack);

void nh_stack_push (NHStack *stack, nhptr data);
[[nodiscard]] nhptr nh_stack_pop (NHStack *stack);

[[nodiscard]] nhbool nh_stack_empty (NHStack *stack);
