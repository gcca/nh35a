#include <stddef.h>

#include "btree.h"

#include "mem.h"

typedef struct NH_BTree_ NH_BTree;
typedef void (*NHBTreeInsert) (NH_BTree *tree, NHBTreeCmp cmp, nhptr key,
                               nhptr value);

struct NH_BTree_
{
  NHBTreeNode *root;
  NHBTreeCmp cmp;
  NHBTreeInsert insert;
};

void
nh_btree_insert_node (NHBTreeNode **place, NHBTreeNode *node, NHBTreeCmp cmp,
                      nhptr key, nhptr value)
{
  if (node == NULL)
    {
      NHBTreeNode *new_node = nh_malloc (sizeof (NHBTreeNode));
      new_node->left = NULL;
      new_node->right = NULL;
      new_node->key = key;
      new_node->value = value;
      *place = new_node;
      return;
    }
  nhint cmp_v = cmp (key, node->key);
  if (cmp_v == 0)
    {
      node->value = value;
      return;
    }
  if (cmp_v < 0)
    {
      nh_btree_insert_node (&node->left, node->left, cmp, key, value);
      return;
    }
  nh_btree_insert_node (&node->right, node->right, cmp, key, value);
}

void
nh_btree_insert_start (NH_BTree *tree, NHBTreeCmp cmp, nhptr key, nhptr value)
{
  NHBTreeNode *root = tree->root;
  nhint cmp_v = cmp (key, root->key);
  if (cmp_v == 0)
    {
      root->value = value;
      return;
    }
  if (cmp_v < 0)
    {
      nh_btree_insert_node (&root->left, root->left, cmp, key, value);
      return;
    }
  nh_btree_insert_node (&root->right, root->right, cmp, key, value);
}

void
nh_btree_insert_root (NH_BTree *tree, NHBTreeCmp cmp, nhptr key, nhptr value)
{
  NHBTreeNode *node = nh_malloc (sizeof (NHBTreeNode));
  node->left = NULL;
  node->right = NULL;
  node->key = key;
  node->value = value;
  tree->root = node;
  tree->insert = nh_btree_insert_start;
}

NHBTree *
nh_btree_new (NHBTreeCmp cmp)
{
  NH_BTree *tree = nh_malloc (sizeof (NH_BTree));
  tree->root = NULL;
  tree->cmp = cmp;
  tree->insert = nh_btree_insert_root;
  return (NHBTree *)tree;
}

void
nh_btree_free (NHBTree *tree)
{
  nh_free (tree);
}

void
nh_btree_insert (NHBTree *tree_, nhptr key, nhptr value)
{
  NH_BTree *tree = (NH_BTree *)tree_;
  tree->insert (tree, tree->cmp, key, value);
}
