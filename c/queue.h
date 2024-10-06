#include "types.h"

typedef struct NHQueue_ NHQueue;
typedef struct NHQueueNode_ NHQueueNode;

struct NHQueue_
{
  struct NHQueueNode_
  {
    nhptr data;
    NHQueueNode *next;
  } *root;
};

[[nodiscard]] NHQueue *nh_queue_new ();
void nh_queue_free (NHQueue *queue);

void nh_queue_push (NHQueue *queue, nhptr data);
[[nodiscard]] nhptr nh_queue_pop (NHQueue *queue);

[[nodiscard]] nhbool nh_queue_empty (NHQueue *queue);
