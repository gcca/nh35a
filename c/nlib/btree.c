#include <stddef.h>

#include "btree.h"

#include "mem.h"
#include "queue.h"
#include "types.h"

typedef struct NH_BTree_ NH_BTree;
typedef struct NH_BTreeNode_ NH_BTreeNode;
typedef struct NH_BTreeTraverseInfo_ NH_BTreeTraverseInfo;

struct NH_BTree_
{
  struct NH_BTreeNode_
  {
    NH_BTreeNode *left;
    NH_BTreeNode *right;
    nhptr key;
    nhptr value;
    nhint bal;
  } *root;
  NHBTreeCmp cmp;
};

struct NH_BTreeTraverseInfo_
{
  nhint level;
  nhint position;
  NHBTreeNode *node;
};

[[nodiscard]] static NH_BTreeNode *
nh_btree_node_rotate_left (NH_BTreeNode *node)
{
  NH_BTreeNode *right = node->right;
  node->right = right->left;
  right->left = node;

  nhint nbal = node->bal;
  nhint rbal = right->bal;

  if (rbal <= 0)
    {
      if (nbal >= 1)
        right->bal = rbal - 1;
      else
        right->bal = nbal + rbal - 2;
      node->bal = nbal - 1;
    }
  else
    {
      if (nbal <= rbal)
        right->bal = nbal - 2;
      else
        right->bal = rbal - 1;
      node->bal = nbal - rbal - 1;
    }

  return right;
}

static NH_BTreeNode *
nh_btree_node_rotate_right (NH_BTreeNode *node)
{
  NH_BTreeNode *left = node->left;
  node->left = left->right;
  left->right = node;

  nhint nbal = node->bal;
  nhint lbal = left->bal;

  if (lbal <= 0)
    {
      if (lbal >= nbal)
        left->bal = lbal + 1;
      else
        left->bal = nbal + 2;
      node->bal = nbal - lbal + 1;
    }
  else
    {
      if (nbal <= -1)
        left->bal = lbal + 1;
      else
        left->bal = nbal + lbal + 2;
      node->bal = nbal + 1;
    }

  return left;
}

inline static NH_BTreeNode *
nh_btree_node_new (nhptr key, nhptr value)
{
  NH_BTreeNode *node = nh_malloc (sizeof (NH_BTreeNode));
  node->left = nullptr;
  node->right = nullptr;
  node->key = key;
  node->value = value;
  node->bal = 0;
  return node;
}

static NH_BTreeNode *
nh_btree_node_bal (NH_BTreeNode *node)
{
  if (node->bal < -1)
    {
      if (node->left->bal > 0)
        node->left = nh_btree_node_rotate_left (node->left);
      return nh_btree_node_rotate_right (node);
    }
  else if (node->bal > 1)
    {
      if (node->right->bal < 0)
        node->right = nh_btree_node_rotate_right (node->right);
      return nh_btree_node_rotate_left (node);
    }
  return node;
}

static NH_BTreeNode *
nh_btree_node_insert (NH_BTreeNode *node, NHBTreeCmp cmp, nhptr key,
                      nhptr value, nhbool *inserted)
{
  if (!node)
    {
      *inserted = true;
      return nh_btree_node_new (key, value);
    }
  nhint cmp_v = cmp (key, node->key);
  if (cmp_v == 0)
    {
      node->value = value;
      *inserted = false;
      return node;
    }
  if (cmp_v < 0)
    {
      if (node->left)
        {
          nhint bal = node->left->bal;
          node->left
              = nh_btree_node_insert (node->left, cmp, key, value, inserted);
          if ((bal != node->left->bal) && node->left->bal) node->bal -= 1;
        }
      else
        {
          node->left = nh_btree_node_new (key, value);
          node->bal -= 1;
          *inserted = true;
        }
    }
  else if (node->right)
    {
      nhint bal = node->right->bal;
      node->right
          = nh_btree_node_insert (node->right, cmp, key, value, inserted);
      if ((bal != node->right->bal) && node->right->bal) node->bal += 1;
    }
  else
    {
      node->right = nh_btree_node_new (key, value);
      node->bal += 1;
      *inserted = true;
    }
  if (*inserted)
    if (node->bal > 1 || node->bal < -1) node = nh_btree_node_bal (node);
  return node;
}

static NH_BTreeNode *
nh_btree_node_restore_left_bal (NH_BTreeNode *node, nhint bal)
{
  if (!node->left)
    node->bal++;
  else if ((node->left->bal != bal) && (node->left->bal == 0))
    node->bal++;
  if (node->bal > 1) return nh_btree_node_bal (node);
  return node;
}

static NH_BTreeNode *
nh_btree_node_restore_right_bal (NH_BTreeNode *node, nhint bal)
{
  if (!node->right)
    node->bal--;
  else if ((node->right->bal != bal) && (node->right->bal == 0))
    node->bal--;
  if (node->bal < -1) return nh_btree_node_bal (node);
  return node;
}

static NH_BTreeNode *
nh_btree_node_remove_lmost (NH_BTreeNode *node, NH_BTreeNode **lmost)
{
  if (!node->left)
    {
      *lmost = node;
      return node->right;
    }

  nhint bal = node->left->bal;
  node->left = nh_btree_node_remove_lmost (node->left, lmost);
  return nh_btree_node_restore_left_bal (node, bal);
}

static NH_BTreeNode *
nh_btree_node_remove (NH_BTreeNode *node, NHBTreeCmp cmp, nhptr key)
{

  if (!node) return nullptr;

  nhint cmp_v = cmp (key, node->key);
  NH_BTreeNode *garbage = nullptr;
  if (cmp_v == 0)
    {
      garbage = node;
      if (!node->right)
        node = node->left;
      else
        {
          nhint bal = node->right->bal;
          NH_BTreeNode *root;
          node->right = nh_btree_node_remove_lmost (node->right, &root);
          root->left = node->left;
          root->right = node->right;
          root->bal = node->bal;
          node = nh_btree_node_restore_right_bal (root, bal);
        }
      nh_free (garbage);
    }
  else if (cmp_v < 0)
    {
      if (node->left)
        {
          nhint bal = node->left->bal;
          node->left = nh_btree_node_remove (node->left, cmp, key);
          node = nh_btree_node_restore_left_bal (node, bal);
        }
    }
  else
    {
      if (node->right)
        {
          nhint bal = node->right->bal;
          node->right = nh_btree_node_remove (node->right, cmp, key);
          node = nh_btree_node_restore_right_bal (node, bal);
        }
    }

  return node;
}

NHBTree *
nh_btree_new (NHBTreeCmp cmp)
{
  NH_BTree *tree = nh_malloc (sizeof (NH_BTree));
  tree->root = nullptr;
  tree->cmp = cmp;
  return (NHBTree *)tree;
}

static void
nb_btree_node_free (NHBTreeNode *node)
{
  if (!node) return;
  nb_btree_node_free (node->left);
  nb_btree_node_free (node->right);
  nh_free (node);
}

void
nh_btree_free (NHBTree *tree)
{
  nb_btree_node_free (tree->root);
  nh_free (tree);
}

void
nh_btree_insert (NHBTree *tree_, nhptr key, nhptr value)
{
  NH_BTree *tree = (NH_BTree *)tree_;
  nhbool inserted = false;
  tree->root
      = nh_btree_node_insert (tree->root, tree->cmp, key, value, &inserted);
}

void
nh_btree_remove (NHBTree *tree_, nhptr key)
{
  NH_BTree *tree = (NH_BTree *)tree_;
  tree->root = nh_btree_node_remove (tree->root, tree->cmp, key);
}

nhptr
nh_btree_node_search_data (NHBTreeNode *node, NHBTreeSearch search, nhptr data)
{
  if (!node) return nullptr;

  nhint d;
  do {
      d = search (node->key, data);
      if (d == 0) return node->value;
      node = d < 0 ? node->left : node->right;
    }
  while (node && (d != 0));

  return nullptr;
}

nhptr
nh_btree_search_data (NHBTree *tree, NHBTreeSearch search, nhptr data)
{
  return nh_btree_node_search_data (tree->root, search, data);
}

static nhbool
nh_btree_node_pre_traverse (NH_BTreeNode *node, NHBTreeTraverser traverser)
{
  if (traverser (node->key, node->value)) return true;
  if (node->left)
    if (nh_btree_node_pre_traverse (node->left, traverser)) return true;
  if (node->right)
    if (nh_btree_node_pre_traverse (node->right, traverser)) return true;
  return false;
}

void
nh_btree_pre_traverse (NHBTree *tree, NHBTreeTraverser traverser)
{
  nh_btree_node_pre_traverse (((NH_BTree *)tree)->root, traverser);
}

static nhbool
nh_btree_node_in_traverse (NH_BTreeNode *node, NHBTreeTraverser traverser)
{
  if (node->left)
    if (nh_btree_node_in_traverse (node->left, traverser)) return true;
  if (traverser (node->key, node->value)) return true;
  if (node->right)
    if (nh_btree_node_in_traverse (node->right, traverser)) return true;
  return false;
}

void
nh_btree_in_traverse (NHBTree *tree, NHBTreeTraverser traverser)
{
  nh_btree_node_in_traverse (((NH_BTree *)tree)->root, traverser);
}

static nhbool
nh_btree_node_post_traverse (NH_BTreeNode *node, NHBTreeTraverser traverser)
{
  if (node->left)
    if (nh_btree_node_post_traverse (node->left, traverser)) return true;
  if (node->right)
    if (nh_btree_node_post_traverse (node->right, traverser)) return true;
  if (traverser (node->key, node->value)) return true;
  return false;
}

void
nh_btree_post_traverse (NHBTree *tree, NHBTreeTraverser traverser)
{
  nh_btree_node_post_traverse (((NH_BTree *)tree)->root, traverser);
}

void
nh_btree_level_traverse (NHBTree *tree, NHBTreeTraverser traverser)
{
  NHQueue *queue = nh_queue_new ();
  nh_queue_push (queue, tree->root);

  while (!nh_queue_empty (queue))
    {
      NH_BTreeNode *node = nh_queue_pop (queue);
      if (traverser (node->key, node->value)) return;
      if (node->left) nh_queue_push (queue, node->left);
      if (node->right) nh_queue_push (queue, node->right);
    }

  nh_queue_free (queue);
}

void
nh_btree_level_info_traverse (NHBTree *tree, NHBTreeInfoTraverser traverser)
{
  NHQueue *queue = nh_queue_new ();
  NH_BTreeTraverseInfo infos[50];
  nhint infos_count = 0;

  infos[0] = (NH_BTreeTraverseInfo){ .level = 0,
                                     .position = 0,
                                     .node = tree->root };
  infos_count++;

  nh_queue_push (queue, &infos[0]);

  while (!nh_queue_empty (queue))
    {
      NH_BTreeTraverseInfo *info = nh_queue_pop (queue);
      NHBTreeNode *node = info->node;
      if (traverser (node->key, node->value, (NHBTreeTraverseInfo *)info))
        return;
      if (node->left)
        {
          infos[infos_count]
              = (NH_BTreeTraverseInfo){ .level = info->level + 1,
                                        .position = 2 * info->position,
                                        .node = node->left };
          nh_queue_push (queue, &infos[infos_count++]);
        }
      if (node->right)
        {
          infos[infos_count]
              = (NH_BTreeTraverseInfo){ .level = info->level + 1,
                                        .position = 2 * info->position + 1,
                                        .node = node->right };
          nh_queue_push (queue, &infos[infos_count++]);
        }
    }

  nh_queue_free (queue);
}
