#include "stack.h"
#include "mem.h"
#include "types.h"

typedef struct NH_Stack_ NH_Stack;

struct NH_Stack_
{
  NHStackNode *root;
  NHSChunk *chunk;
};

NHStack *
nh_stack_new ()
{
  NH_Stack *stack = nh_new (NH_Stack);

  stack->chunk
      = nh_schunk_new (sizeof (NHStackNode), sizeof (NHStackNode) * 1024);
  stack->root = nh_schunk_alloc (stack->chunk);
  stack->root->data = nullptr;
  stack->root->next = stack->root;

  return (NHStack *)stack;
}

void
nh_stack_free (NHStack *stack_)
{
  NH_Stack *stack = (NH_Stack *)stack_;
  nh_schunk_free (stack->chunk);
  nh_free (stack);
}

void
nh_stack_push (NHStack *stack_, nhptr data)
{
  NH_Stack *stack = (NH_Stack *)stack_;

  NHStackNode *node = nh_schunk_alloc (stack->chunk);

  node->data = data;
  node->next = stack->root->next;
  stack->root->next = node;
}

nhptr
nh_stack_pop (NHStack *stack_)
{
  NH_Stack *stack = (NH_Stack *)stack_;
  NHStackNode *node = stack->root->next;

  nhptr data = node->data;
  stack->root->next = node->next;

  nh_schunk_dealloc (stack->chunk, node);

  return data;
}

nhbool
nh_stack_empty (NHStack *stack)
{
  return stack->root->next == stack->root;
}
