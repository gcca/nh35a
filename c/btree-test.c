#include "mem.h"
#include "nh35a_test.h"

nhint ZKEYS[] = { 10, 20, 30, 40, 50, 60 };
nhint ZVALUES[] = { 20, 21, 22, 23, 24, 25 };

nhbool
test_traverser (nhptr key, nhptr value)
{
  nhint k = *(nhint *)key;
  nhint v = *(nhint *)value;
  nh_print ("[%d]: %d\n", k, v);
  return FALSE;
}

nhbool
test_traverser_with_info (nhptr key, nhptr value,
                          const NHBTreeTraverseInfo *info)
{
  nhint k = *(nhint *)key;
  nhint v = *(nhint *)value;
  nh_print ("(%d,%d) [%d]: %d\n", info->level, info->position, k, v);
  return FALSE;
}

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

NH_TEST (insert_basic)
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
  nh_assert (tree->root->right->key == ZKEYS + 5);
  nh_assert (tree->root->right->value == ZVALUES + 5);

  nh_assert (tree->root->left->left != NULL);
  nh_assert (tree->root->left->left->key == ZKEYS + 1);
  nh_assert (tree->root->left->left->value == ZVALUES + 1);

  nh_assert (tree->root->right->left != NULL);
  nh_assert (tree->root->right->left->key == ZKEYS + 4);
  nh_assert (tree->root->right->left->value == ZVALUES + 4);

  nhuint key = 25;
  nhuint value = 26;
  nh_btree_insert (tree, &key, &value);

  nh_assert (tree->root->left->left->right != NULL);
  nh_assert (tree->root->left->left->right->key == &key);
  nh_assert (tree->root->left->left->right->value == &value);

  nh_btree_free (tree);
}

NH_TEST (preorder)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_preorder_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (inorder)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_inorder_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (postorder)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_postorder_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (levelorder)
{
  NHBTree *tree = nh_btree_new (zcmp);

  nh_btree_insert (tree, ZKEYS + 3, ZVALUES + 3);
  nh_btree_insert (tree, ZKEYS + 4, ZVALUES + 4);
  nh_btree_insert (tree, ZKEYS + 2, ZVALUES + 2);
  nh_btree_insert (tree, ZKEYS + 1, ZVALUES + 1);
  nh_btree_insert (tree, ZKEYS + 5, ZVALUES + 5);

  nh_btree_levelorder_traverse (tree, test_traverser);

  nh_btree_free (tree);
}

NH_TEST (levelorder_info)
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

  nh_btree_levelorder_info_traverse (tree, test_traverser_with_info);

  nh_btree_free (tree);
}
