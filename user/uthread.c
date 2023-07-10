#include "uthread.h"
#include "user.h"

struct uthread uthreads[MAX_UTHREADS];
struct uthread* this_uthread;

int uthread_create(void (*start_func)(), enum sched_priority priority) {
  struct uthread* th;
  uint8 found = 0;

  for (th = uthreads; th < &uthreads[MAX_UTHREADS]; th++) {
    if (th->state == FREE) {
      found = 1;
      break;
    }
  }
  
  if (!found) {
    return -1;
  }

  th->context.ra = (uint64)start_func;
  th->context.sp = (uint64)th->ustack + STACK_SIZE;
  th->state = RUNNABLE;
  th->priority = priority;

  return 0;
}

// Must only be called from uthreads.
// returns a pointer to this thread's TCB
// finds it by checking the SP (the SP is contained in the TCB)
struct uthread* uthread_self() {
  int idx = 0; // Create an integer on the stack, so we can get a
  uint64 sp = (uint64)&idx;

  idx = (sp - (uint64)uthreads) / sizeof(struct uthread);

  return uthreads + idx;
}

int num_runnable_prio(enum sched_priority prio) {
  int count = 0;
  struct uthread* th;

  for (th = uthreads; th < &uthreads[MAX_UTHREADS]; th++) {
    if (th->priority == prio && th->state == RUNNABLE) {
      count++;
    }
  }

  return count;
} 

// Chooses the next thread to run (roundrobin).
// If there are no runnable threads, exit.
struct uthread* choose() {
  static struct uthread* th[3] = {uthreads, uthreads, uthreads};
  enum sched_priority prio;
  struct uthread* chosen;

  for (prio = HIGH; prio >= LOW; prio--) {
    if (num_runnable_prio(prio) == 0) {
      continue;
    }

    for (;;) {
      if (th[prio] == &uthreads[MAX_UTHREADS]) {
        th[prio] = uthreads;
      }

      if (th[prio]->priority == prio && th[prio]->state == RUNNABLE) {
        chosen = th[prio];
        th[prio]++;
        return chosen;
      }

      th[prio]++;
    }
  }
}

void uthread_yield() {
  struct uthread* this = uthread_self();
  struct uthread* next;


  this->state = RUNNABLE;
  next = choose();

  next->state = RUNNING;
  uswtch(&this->context, &next->context);
}

int num_runnable_running() {
  int count = 0;
  struct uthread* th;

  for (th = uthreads; th < &uthreads[MAX_UTHREADS]; th++) {
    if (th->state == RUNNABLE || th->state == RUNNING) {
      count++;
    }
  }

  return count;
}

void uthread_exit() {
  if (num_runnable_running() == 1) { // exit if this is the last thread
    exit(0);
  }

  struct uthread* this = uthread_self();
  struct uthread* next;

  this->state = FREE;
  next = choose();

  next->state = RUNNING;
  uswtch(&this->context, &next->context);
}


// Doesn't return.
int uthread_start_all() {
  struct uthread* next_thread = choose();
  struct context mock_context = {0};

  next_thread->state = RUNNING;
  uswtch(&mock_context, &next_thread->context);

  return -1;
}

enum sched_priority uthread_get_priority() {
  return uthread_self()->priority;
}

enum sched_priority uthread_set_priority(enum sched_priority priority) {
  struct uthread* self = uthread_self();
  enum sched_priority prev = self->priority;
  self->priority = priority;
  return prev;
}
