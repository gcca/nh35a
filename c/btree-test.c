#include "mem.h"
#include "nh35a_test.h"

nhint ZKEYS[] = { 10, 20, 30, 40, 50, 60 };
nhint ZVALUES[] = { 20, 21, 22, 23, 24, 25 };

nhchar stream_buf[128];
NHSStream stream = { stream_buf, stream_buf, 0 };

nhbool
test_traverser (nhptr key, nhptr value)
{
  nhint k = *(nhint *)key;
  nhint v = *(nhint *)value;

  nh_sstream (&stream, (char)'[');
  nh_sstream (&stream, k);
  nh_sstream (&stream, "]: ");
  nh_sstream (&stream, v);
  nh_sstream_cout_ln (&stream);

  return false;
}

nhbool
test_traverser_with_info (nhptr key, nhptr value,
                          const NHBTreeTraverseInfo *info)
{
  nhint k = *(nhint *)key;
  nhint v = *(nhint *)value;

  nh_sstream (&stream, (char)'(');
  nh_sstream (&stream, info->level);
  nh_sstream (&stream, (char)',');
  nh_sstream (&stream, info->position);
  nh_sstream (&stream, ") [");
  nh_sstream (&stream, k);
  nh_sstream (&stream, "]: ");
  nh_sstream (&stream, v);
  nh_sstream_cout_ln (&stream);

  return false;
}

nhint
test_search_data (nhptr a, nhptr b)
{
  return b - a;
}

nhint
zcmp (nhptr a, nhptr b)
{
  return *(nhint *)a - *(nhint *)b;
}

NH_TEST (new)
{
  NHBTree *tree = nh_btree_new (nullptr);
  nh_assert (tree != nullptr);
  nh_btree_free (tree);
}

NH_TEST (insert_single)
{
  NHBTree *tree = nh_btree_new (nullptr);

  nh_btree_insert (tree, ZKEYS, ZVALUES);

  nh_assert (tree->root != nullptr);
  nh_assert (tree->root->key == ZKEYS);
  nh_assert (tree->root->value == ZVALUES);

  nh_btree_free (tree);
}

NH_TEST (insert_basic)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_assert (tree->root != nullptr);
  nh_assert (tree->root->key == ZKEYS + 3);
  nh_assert (tree->root->value == ZVALUES + 3);

  nh_assert (tree->root->left != nullptr);
  nh_assert (tree->root->left->key == ZKEYS + 2);
  nh_assert (tree->root->left->value == ZVALUES + 2);

  nh_assert (tree->root->right != nullptr);
  nh_assert (tree->root->right->key == ZKEYS + 4);
  nh_assert (tree->root->right->value == ZVALUES + 4);

  nh_assert (tree->root->left->left != nullptr);
  nh_assert (tree->root->left->left->key == ZKEYS + 1);
  nh_assert (tree->root->left->left->value == ZVALUES + 1);

  nh_assert (tree->root->right->right != nullptr);
  nh_assert (tree->root->right->right->key == ZKEYS + 5);
  nh_assert (tree->root->right->right->value == ZVALUES + 5);

  nhuint key = 25;
  nhuint value = 26;
  nh_btree_insert (tree, &key, &value);

  nh_assert (tree->root != nullptr);
  nh_assert (tree->root->key == ZKEYS + 3);
  nh_assert (tree->root->value == ZVALUES + 3);

  nh_assert (tree->root->left != nullptr);
  nh_assert (tree->root->left->key == &key);
  nh_assert (tree->root->left->value == &value);

  nh_assert (tree->root->right != nullptr);
  nh_assert (tree->root->right->key == ZKEYS + 4);
  nh_assert (tree->root->right->value == ZVALUES + 4);

  nh_assert (tree->root->left->left != nullptr);
  nh_assert (tree->root->left->left->key == ZKEYS + 1);
  nh_assert (tree->root->left->left->value == ZVALUES + 1);

  nh_assert (tree->root->left->right != nullptr);
  nh_assert (tree->root->left->right->key == ZKEYS + 2);
  nh_assert (tree->root->left->right->value == ZVALUES + 2);

  nh_assert (tree->root->right->right != nullptr);
  nh_assert (tree->root->right->right->key == ZKEYS + 5);
  nh_assert (tree->root->right->right->value == ZVALUES + 5);

  nh_btree_free (tree);
}

NH_TEST (remove)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_remove (tree, ZKEYS + 3);

  nh_assert (tree->root != nullptr);
  nh_assert (tree->root->key == ZKEYS + 4);
  nh_assert (tree->root->value == ZVALUES + 4);

  nh_assert (tree->root->left != nullptr);
  nh_assert (tree->root->left->key == ZKEYS + 2);
  nh_assert (tree->root->left->value == ZVALUES + 2);

  nh_assert (tree->root->right != nullptr);
  nh_assert (tree->root->right->key == ZKEYS + 5);
  nh_assert (tree->root->right->value == ZVALUES + 5);

  nh_assert (tree->root->left->left != nullptr);
  nh_assert (tree->root->left->left->key == ZKEYS + 1);
  nh_assert (tree->root->left->left->value == ZVALUES + 1);

  nh_btree_free (tree);
}

NH_TEST (search_data)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);

  nh_assert (nh_btree_search_data (tree, test_search_data, ZKEYS + 2)
             == ZVALUES + 2);
  nh_assert (nh_btree_search_data (tree, test_search_data, ZKEYS + 5)
             == nullptr);

  nh_btree_free (tree);
}

NH_TEST (pre)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_pre_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (in)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_in_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (post)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_post_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (level)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_level_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (level_info)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 0, ZVALUES + 0);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nhint XKEYS[] = { 70, 80, 90, 100, 100, 120 };
  nhint XVALUES[] = { 26, 27, 28, 29, 30, 31 };

  nh_btree_insert (tree, XKEYS + 0, XVALUES + 0);
  nh_btree_insert (tree, XKEYS + 1, XVALUES + 1);
  nh_btree_insert (tree, XKEYS + 2, XVALUES + 2);

  nh_btree_level_info_traverse (tree, test_traverser_with_info);

  nh_btree_free (tree);
}
