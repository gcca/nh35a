#include "nh35a_test.h"

nhint ZVALUES[] = { 10, 11, 12, 13, 14, 15 };

NH_TEST (push)
{
  NHQueue *queue = nh_queue_new ();

  nh_queue_push (queue, ZVALUES + 0);
  nh_queue_push (queue, ZVALUES + 1);
  nh_queue_push (queue, ZVALUES + 2);

  nh_assert (queue->root->next->data == ZVALUES + 0);
  nh_assert (queue->root->next->next->data == ZVALUES + 1);
  nh_assert (queue->root->next->next->next->data == ZVALUES + 2);

  nh_queue_free (queue);
}

NH_TEST (pop)
{
  NHQueue *queue = nh_queue_new ();

  nh_queue_push (queue, ZVALUES + 0);
  nh_queue_push (queue, ZVALUES + 1);
  nh_queue_push (queue, ZVALUES + 2);

  nh_assert (nh_queue_pop (queue) == ZVALUES + 0);
  nh_assert (nh_queue_pop (queue) == ZVALUES + 1);
  nh_assert (nh_queue_pop (queue) == ZVALUES + 2);

  nh_queue_free (queue);
}

NH_TEST (push_after_empty)
{
  NHQueue *queue = nh_queue_new ();

  nh_queue_push (queue, ZVALUES + 0);
  nh_queue_push (queue, ZVALUES + 1);
  nh_queue_push (queue, ZVALUES + 2);

  nh_assert (nh_queue_pop (queue) == ZVALUES + 0);
  nh_assert (nh_queue_pop (queue) == ZVALUES + 1);
  nh_assert (nh_queue_pop (queue) == ZVALUES + 2);

  nh_queue_push (queue, ZVALUES + 3);
  nh_queue_push (queue, ZVALUES + 4);

  nh_assert (nh_queue_pop (queue) == ZVALUES + 3);
  nh_assert (nh_queue_pop (queue) == ZVALUES + 4);

  nh_queue_free (queue);
}

NH_TEST (empty)
{
  NHQueue *queue = nh_queue_new ();

  nh_assert (nh_queue_empty (queue));

  nh_queue_push (queue, ZVALUES + 0);
  nh_assert (!nh_queue_empty (queue));

  nh_assert (nh_queue_pop (queue) == ZVALUES + 0);
  nh_assert (nh_queue_empty (queue));

  nh_queue_free (queue);
}
