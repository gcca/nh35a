#include "nh35a_test.h"

nhint ZVALUES[] = { 10, 11, 12, 13, 14, 15 };

nhchar stream_buf[128];
NHSStream stream = { stream_buf, stream_buf, 0 };

void
test_print (nhptr data)
{
  nh_sstream_char (&stream, ' ');
  nh_sstream (&stream, *(int *)data);
}

void
test_show (const char *name, NHSList *list)
{
  nh_sstream (&stream, name);
  nh_slist_foreach (list, test_print);
  nh_sstream_cout_ln (&stream);
}

NH_TEST (new)
{
  NHSList *list = nh_slist_new ();
  nh_assert (list != nullptr);
  nh_slist_free (list);
}

NH_TEST (empty)
{
  NHSList *list = nh_slist_new ();

  nh_assert (nh_slist_empty (list));
  nh_slist_append (list, ZVALUES);
  nh_assert (!nh_slist_empty (list));

  nh_slist_free (list);
}

NH_TEST (length)
{
  NHSList *list = nh_slist_new ();

  nh_assert (nh_slist_length (list) == 0);

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);

  nh_assert (nh_slist_length (list) == 3);

  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  nh_assert (nh_slist_length (list) == 5);

  test_show ("length:", list);

  nh_slist_free (list);
}

NH_TEST (last)
{
  NHSList *list = nh_slist_new ();

  nh_assert (nh_slist_last (list) == list->root);

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);

  nh_assert (nh_slist_last (list)->data == ZVALUES + 3);

  nh_slist_append (list, ZVALUES + 4);
  nh_slist_append (list, ZVALUES + 5);

  nh_assert (nh_slist_last (list)->data == ZVALUES + 5);

  test_show ("last:", list);

  nh_slist_free (list);
}

NH_TEST (append)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  nh_assert (nh_slist_length (list) == 5);

  test_show ("append:", list);

  nh_slist_free (list);
}

NH_TEST (prepend)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_prepend (list, ZVALUES + 4);

  nh_assert (nh_slist_length (list) == 5);

  nh_assert (nh_slist_last (list)->data == ZVALUES + 3);
  nh_assert (list->root->next->data == ZVALUES + 4);

  test_show ("prepend:", list);

  nh_slist_free (list);
}

NH_TEST (nth)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  nh_assert (nh_slist_nth (list, 0)->data == ZVALUES + 0);
  nh_assert (nh_slist_nth (list, 1)->data == ZVALUES + 1);
  nh_assert (nh_slist_nth (list, 2)->data == ZVALUES + 2);
  nh_assert (nh_slist_nth (list, 3)->data == ZVALUES + 3);
  nh_assert (nh_slist_nth (list, 4)->data == ZVALUES + 4);

  nh_slist_free (list);
}

NH_TEST (remove)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  test_show ("remove:", list);

  nh_assert (nh_slist_length (list) == 5);

  nh_slist_remove (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 5);

  test_show ("       ", list);

  nh_assert (nh_slist_length (list) == 5);

  nh_slist_free (list);
}

NH_TEST (top)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  test_show ("top:", list);

  nh_assert (nh_slist_length (list) == 5);
  nh_assert (nh_slist_top (list) == ZVALUES + 0);
  nh_assert (nh_slist_top (list) == ZVALUES + 1);
  nh_assert (nh_slist_top (list) == ZVALUES + 2);
  nh_assert (nh_slist_length (list) == 2);

  test_show ("    ", list);

  nh_slist_free (list);
}

NH_TEST (tail)
{
  NHSList *list = nh_slist_new ();

  nh_slist_append (list, ZVALUES + 0);
  nh_slist_append (list, ZVALUES + 1);
  nh_slist_append (list, ZVALUES + 2);
  nh_slist_append (list, ZVALUES + 3);
  nh_slist_append (list, ZVALUES + 4);

  test_show ("tail:", list);

  nh_assert (nh_slist_length (list) == 5);
  nh_assert (nh_slist_tail (list) == ZVALUES + 4);
  nh_assert (nh_slist_tail (list) == ZVALUES + 3);
  nh_assert (nh_slist_tail (list) == ZVALUES + 2);
  nh_assert (nh_slist_length (list) == 2);

  test_show ("     ", list);

  nh_slist_free (list);
}
