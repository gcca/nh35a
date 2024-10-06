#include "types.h"

typedef struct NHBTree_ NHBTree;
typedef struct NHBTreeNode_ NHBTreeNode;

typedef nhint (*NHBTreeCmp) (nhptr a, nhptr b);

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

NHBTree *nh_btree_new (NHBTreeCmp cmp);
void nh_btree_free (NHBTree *tree);

void nh_btree_insert (NHBTree *tree, nhptr key, nhptr value);
