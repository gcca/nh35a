#include "slist.h"

#include "mem.h"

typedef struct NH_SList_ NH_SList;

struct NH_SList_
{
  NHSListNode *root;
  NHSChunk *chunk;
};

NHSList *
nh_slist_new ()
{
  NH_SList *list = nh_new (NH_SList);

  list->chunk
      = nh_schunk_new (sizeof (NHSListNode), sizeof (NHSListNode) * 1024);
  list->root = nh_schunk_alloc (list->chunk);
  list->root->data = nullptr;
  list->root->next = nullptr;

  return (NHSList *)list;
}

void
nh_slist_free (NHSList *list_)
{
  NH_SList *list = (NH_SList *)list_;
  nh_schunk_free (list->chunk);
  nh_free (list);
}

void
nh_slist_append (NHSList *list_, nhptr data)
{
  NH_SList *list = (NH_SList *)list_;
  NHSListNode *node = nh_slist_last (list_);
  node->next = nh_schunk_alloc (list->chunk);
  node->next->data = data;
  node->next->next = nullptr;
}

void
nh_slist_prepend (NHSList *list_, nhptr data)
{
  NH_SList *list = (NH_SList *)list_;
  NHSListNode *node = nh_schunk_alloc (list->chunk);
  node->data = data;
  node->next = list->root->next;
  list->root->next = node;
}

NHSListNode *
nh_slist_nth (NHSList *list, nhuint n)
{
  NHSListNode *node = list->root->next;
  while (n-- && node) node = node->next;
  return node;
}

void
nh_slist_remove (NHSList *list_, nhptr data)
{
  NH_SList *list = (NH_SList *)list_;
  NHSListNode *prev = list->root, *node = list->root->next;
  while (node)
    {
      if (node->data == data)
        {
          prev->next = node->next;
          nh_schunk_dealloc (list->chunk, node);
          return;
        }
      prev = node;
      node = node->next;
    }
}

nhptr
nh_slist_top (NHSList *list)
{
  NHSListNode *node = list->root->next;

  list->root->next = node->next;
  nhptr data = node->data;
  nh_schunk_dealloc (((NH_SList *)list)->chunk, node);

  return data;
}

nhptr
nh_slist_tail (NHSList *list)
{
  NHSListNode *prev = list->root, *node = list->root->next;
  while (node->next)
    {
      prev = node;
      node = node->next;
    }

  nhptr data = node->data;
  prev->next = nullptr;
  nh_schunk_dealloc (((NH_SList *)list)->chunk, node);

  return data;
}

void
nh_slist_foreach (NHSList *list, NHSListFunc func)
{
  NHSListNode *node = list->root->next;
  while (node)
    {
      func (node->data);
      node = node->next;
    }
}

NHSListNode *
nh_slist_last (NHSList *list)
{
  NHSListNode *node = list->root;
  while (node->next) node = node->next;
  return node;
}

nhbool
nh_slist_empty (NHSList *list)
{
  return list->root->next == nullptr;
}

nhuint
nh_slist_length (NHSList *list)
{
  nhuint length = 0;
  NHSListNode *node = list->root;
  while (node->next)
    {
      length++;
      node = node->next;
    }
  return length;
}
