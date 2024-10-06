#include "types.h"

typedef struct NHSList_ NHSList;
typedef struct NHSListNode_ NHSListNode;

typedef void (*NHSListFunc) (nhptr data);

struct NHSList_
{
  struct NHSListNode_
  {
    nhptr data;
    NHSListNode *next;
  } *root;
};

NHSList *nh_slist_new ();
void nh_slist_free (NHSList *list);

void nh_slist_append (NHSList *list, nhptr data);
void nh_slist_prepend (NHSList *list, nhptr data);

NHSListNode *nh_slist_nth (NHSList *list, nhuint n);

void nh_slist_remove (NHSList *list, nhptr data);
nhptr nh_slist_top (NHSList *list);
nhptr nh_slist_tail (NHSList *list);

void nh_slist_foreach (NHSList *list, NHSListFunc func);

nhbool nh_slist_empty (NHSList *list);
nhuint nh_slist_length (NHSList *list);
NHSListNode *nh_slist_last (NHSList *list);
