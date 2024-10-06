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

NHQueue *nh_queue_new ();
void nh_queue_free (NHQueue *queue);

void nh_queue_push (NHQueue *queue, nhptr data);
nhptr nh_queue_pop (NHQueue *queue);

nhbool nh_queue_empty (NHQueue *queue);
