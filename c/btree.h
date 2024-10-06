#include "types.h"

typedef struct NHBTree_ NHBTree;
typedef struct NHBTreeNode_ NHBTreeNode;
typedef struct NHBTreeTraverseInfo_ NHBTreeTraverseInfo;

typedef nhint (*NHBTreeCmp) (nhptr a, nhptr b);
typedef nhint (*NHBTreeSearch) (nhptr key, nhptr data);
typedef nhbool (*NHBTreeTraverser) (nhptr key, nhptr value);
typedef nhbool (*NHBTreeInfoTraverser) (nhptr key, nhptr value,
                                        const NHBTreeTraverseInfo *info);

struct NHBTree_
{
  struct NHBTreeNode_
  {
    NHBTreeNode *left;
    NHBTreeNode *right;
    nhptr key;
    nhptr value;
  } *root;
};

struct NHBTreeTraverseInfo_
{
  nhint level;
  nhint position;
};

[[nodiscard]] NHBTree *nh_btree_new (NHBTreeCmp cmp);
void nh_btree_free (NHBTree *tree);

void nh_btree_insert (NHBTree *tree, nhptr key, nhptr value);
void nh_btree_remove (NHBTree *tree, nhptr key);

[[nodiscard]] nhptr nh_btree_search_data (NHBTree *tree, NHBTreeSearch search,
                                          nhptr data);

void nh_btree_pre_traverse (NHBTree *tree, NHBTreeTraverser traverser);
void nh_btree_in_traverse (NHBTree *tree, NHBTreeTraverser traverser);
void nh_btree_post_traverse (NHBTree *tree, NHBTreeTraverser traverser);

void nh_btree_level_traverse (NHBTree *tree, NHBTreeTraverser traverser);
void nh_btree_level_info_traverse (NHBTree *tree,
                                   const NHBTreeInfoTraverser info);
