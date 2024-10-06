#include "types.h"

typedef struct NHTimer_ NHTimer;

struct NHTimer_
{
};

NHTimer *nh_timer_new (void);
void nh_timer_free (NHTimer *timer);
void nh_timer_start (NHTimer *timer);
void nh_timer_stop (NHTimer *timer);
void nh_timer_reset (NHTimer *timer);
nhdouble nh_timer_elapsed_ms (NHTimer *timer);
