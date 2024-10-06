#include <sys/time.h>

#include "mem.h"
#include "timer.h"

typedef struct NH_Timer_ NH_Timer;

struct NH_Timer_
{
  struct timeval start;
  struct timeval end;
  nhbool enabled;
};

NHTimer *
nh_timer_new (void)
{
  NH_Timer *timer = nh_new (NH_Timer);
  timer->enabled = true;
  gettimeofday (&timer->start, nullptr);
  return (NHTimer *)timer;
}

void
nh_timer_free (NHTimer *timer)
{
  nh_free (timer);
}

void
nh_timer_start (NHTimer *timer_)
{
  NH_Timer *timer = (NH_Timer *)timer_;
  gettimeofday (&timer->start, nullptr);
  timer->enabled = true;
}

void
nh_timer_stop (NHTimer *timer_)
{
  NH_Timer *timer = (NH_Timer *)timer_;
  gettimeofday (&timer->end, nullptr);
  timer->enabled = false;
}

void
nh_timer_reset (NHTimer *timer_)
{
  NH_Timer *timer = (NH_Timer *)timer_;
  gettimeofday (&timer->start, nullptr);
}

nhdouble
nh_timer_elapsed_ms (NHTimer *timer_)
{
  NH_Timer *timer = (NH_Timer *)timer_;
  struct timeval elapsed;

  if (timer->enabled) gettimeofday (&timer->end, nullptr);

  if (timer->start.tv_usec > timer->end.tv_usec)
    {
      timer->end.tv_usec += 1e6;
      timer->end.tv_sec--;
    }

  elapsed.tv_sec = timer->end.tv_sec - timer->start.tv_sec;
  elapsed.tv_usec = timer->end.tv_usec - timer->start.tv_usec;

  return elapsed.tv_sec + ((nhdouble)elapsed.tv_usec / 1e6);
  ;
}

nhdouble
nh_timer_elapsed_us (NHTimer *timer_)
{
  NH_Timer *timer = (NH_Timer *)timer_;
  struct timeval elapsed;

  if (timer->enabled) gettimeofday (&timer->end, nullptr);

  if (timer->start.tv_usec > timer->end.tv_usec)
    {
      timer->end.tv_usec += 1e6;
      timer->end.tv_sec--;
    }

  elapsed.tv_sec = timer->end.tv_sec - timer->start.tv_sec;
  elapsed.tv_usec = timer->end.tv_usec - timer->start.tv_usec;

  return elapsed.tv_usec;
}
