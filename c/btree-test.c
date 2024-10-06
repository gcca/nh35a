#include "nh35a_test.h"

nhint ZKEYS[] = { 10, 11, 12, 13, 14, 15 };
nhint ZVALUES[] = { 20, 21, 22, 23, 24, 25 };

nhint
zcmp (nhptr a, nhptr b)
{
  return *(nhint *)a - *(nhint *)b;
}

NH_TEST (new)
{
  NHBTree *tree = nh_btree_new (NULL);
  nh_assert (tree != NULL);
  nh_btree_free (tree);
}

NH_TEST (insert_single)
{
  NHBTree *tree = nh_btree_new (NULL);

  nh_btree_insert (tree, ZKEYS, ZVALUES);

  nh_assert (tree->root != NULL);
  nh_assert (tree->root->key == ZKEYS);
  nh_assert (tree->root->value == ZVALUES);

  nh_btree_free (tree);
}

NH_TEST (insert_multiple)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_assert (tree->root != NULL);
  nh_assert (tree->root->key == ZKEYS + 3);
  nh_assert (tree->root->value == ZVALUES + 3);

  nh_assert (tree->root->left != NULL);
  nh_assert (tree->root->left->key == ZKEYS + 2);
  nh_assert (tree->root->left->value == ZVALUES + 2);

  nh_assert (tree->root->right != NULL);
  nh_assert (tree->root->right->key == ZKEYS + 4);
  nh_assert (tree->root->right->value == ZVALUES + 4);

  nh_assert (tree->root->left->left != NULL);
  nh_assert (tree->root->left->left->key == ZKEYS + 1);
  nh_assert (tree->root->left->left->value == ZVALUES + 1);

  nh_assert (tree->root->right->right != NULL);
  nh_assert (tree->root->right->right->key == ZKEYS + 5);
  nh_assert (tree->root->right->right->value == ZVALUES + 5);

  nh_btree_free (tree);
}
