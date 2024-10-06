#include "queue.h"
#include "mem.h"
#include "types.h"

typedef struct NH_Queue_ NH_Queue;

struct NH_Queue_
{
  NHQueueNode *root;
  NHAllocator *allocator;
};

NHQueue *
nh_queue_new ()
{
  NH_Queue *queue = nh_new (NH_Queue);

  NHChunk *chunk
      = nh_chunk_new (sizeof (NHQueueNode), sizeof (NHQueueNode) * 1024);
  queue->allocator = nh_allocator_new (chunk);

  queue->root = nh_allocator_alloc (queue->allocator);
  queue->root->data = queue->root;
  queue->root->next = NULL;

  return (NHQueue *)queue;
}

void
nh_queue_free (NHQueue *queue_)
{
  NH_Queue *queue = (NH_Queue *)queue_;
  nh_allocator_free (queue->allocator);
  nh_free (queue);
}

void
nh_queue_push (NHQueue *queue_, nhptr data)
{
  NH_Queue *queue = (NH_Queue *)queue_;

  NHQueueNode *node = nh_allocator_alloc (queue->allocator);

  node->data = data;
  node->next = ((NHQueueNode *)queue->root->data)->next;

  ((NHQueueNode *)queue->root->data)->next = node;
  queue->root->data = node;
}

nhptr
nh_queue_pop (NHQueue *queue_)
{
  NH_Queue *queue = (NH_Queue *)queue_;
  NHQueueNode *node = queue->root->next;

  nhptr data = node->data;
  queue->root->next = node->next;

  if (queue->root->data == node) queue->root->data = queue->root;

  nh_allocator_dealloc (queue->allocator, node);

  return data;
}

nhbool
nh_queue_empty (NHQueue *queue)
{
  return queue->root->next == NULL;
}
