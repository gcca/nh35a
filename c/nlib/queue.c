#include "queue.h"
#include "mem.h"
#include "types.h"

typedef struct NH_Queue_ NH_Queue;

struct NH_Queue_
{
  NHQueueNode *root;
  NHSChunk *chunk;
};

NHQueue *
nh_queue_new ()
{
  NH_Queue *queue = nh_new (NH_Queue);

  queue->chunk
      = nh_schunk_new (sizeof (NHQueueNode), sizeof (NHQueueNode) * 1024);
  queue->root = nh_schunk_alloc (queue->chunk);
  queue->root->data = queue->root;
  queue->root->next = nullptr;

  return (NHQueue *)queue;
}

void
nh_queue_free (NHQueue *queue_)
{
  NH_Queue *queue = (NH_Queue *)queue_;
  nh_schunk_free (queue->chunk);
  nh_free (queue);
}

void
nh_queue_push (NHQueue *queue_, nhptr data)
{
  NH_Queue *queue = (NH_Queue *)queue_;

  NHQueueNode *node = nh_schunk_alloc (queue->chunk);

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

  nh_schunk_dealloc (queue->chunk, node);

  return data;
}

nhbool
nh_queue_empty (NHQueue *queue)
{
  return queue->root->next == nullptr;
}
