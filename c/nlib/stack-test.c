#include "nh35a_test.h"

nhint ZVALUES[] = { 10, 11, 12, 13, 14, 15 };

NH_TEST (push)
{
  NHStack *stack = nh_stack_new ();

  nh_stack_push (stack, ZVALUES + 0);
  nh_assert (stack->root->next->data == ZVALUES + 0);

  nh_stack_push (stack, ZVALUES + 1);
  nh_assert (stack->root->next->data == ZVALUES + 1);
  nh_assert (stack->root->next->next->data == ZVALUES + 0);

  nh_stack_push (stack, ZVALUES + 2);
  nh_assert (stack->root->next->data == ZVALUES + 2);
  nh_assert (stack->root->next->next->data == ZVALUES + 1);
  nh_assert (stack->root->next->next->next->data == ZVALUES + 0);

  nh_stack_free (stack);
}

NH_TEST (pop)
{
  NHStack *stack = nh_stack_new ();

  nh_stack_push (stack, ZVALUES + 0);
  nh_stack_push (stack, ZVALUES + 1);
  nh_stack_push (stack, ZVALUES + 2);

  nh_assert (nh_stack_pop (stack) == ZVALUES + 2);
  nh_assert (nh_stack_pop (stack) == ZVALUES + 1);
  nh_assert (nh_stack_pop (stack) == ZVALUES + 0);

  nh_stack_free (stack);
}

NH_TEST (push_after_empty)
{
  NHStack *stack = nh_stack_new ();

  nh_stack_push (stack, ZVALUES + 0);
  nh_stack_push (stack, ZVALUES + 1);
  nh_stack_push (stack, ZVALUES + 2);

  nh_assert (nh_stack_pop (stack) == ZVALUES + 2);
  nh_assert (nh_stack_pop (stack) == ZVALUES + 1);
  nh_assert (nh_stack_pop (stack) == ZVALUES + 0);

  nh_stack_push (stack, ZVALUES + 3);
  nh_stack_push (stack, ZVALUES + 4);

  nh_assert (nh_stack_pop (stack) == ZVALUES + 4);
  nh_assert (nh_stack_pop (stack) == ZVALUES + 3);

  nh_stack_free (stack);
}

NH_TEST (empty)
{
  NHStack *stack = nh_stack_new ();

  nh_assert (nh_stack_empty (stack));

  nh_stack_push (stack, ZVALUES + 0);
  nh_assert (!nh_stack_empty (stack));

  nh_assert (nh_stack_pop (stack) == ZVALUES + 0);
  nh_assert (nh_stack_empty (stack));

  nh_stack_free (stack);
}
