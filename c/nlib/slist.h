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

[[nodiscard]] NHSList *nh_slist_new ();
void nh_slist_free (NHSList *list);

void nh_slist_append (NHSList *list, nhptr data);
void nh_slist_prepend (NHSList *list, nhptr data);

[[nodiscard]] NHSListNode *nh_slist_nth (NHSList *list, nhuint n);

void nh_slist_remove (NHSList *list, nhptr data);
[[nodiscard]] nhptr nh_slist_top (NHSList *list);
[[nodiscard]] nhptr nh_slist_tail (NHSList *list);

void nh_slist_foreach (NHSList *list, NHSListFunc func);

[[nodiscard]] nhbool nh_slist_empty (NHSList *list);
[[nodiscard]] nhuint nh_slist_length (NHSList *list);
[[nodiscard]] NHSListNode *nh_slist_last (NHSList *list);
