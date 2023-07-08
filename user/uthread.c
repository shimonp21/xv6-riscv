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

// Chooses the next thread to run (roundrobin).
// If there are no runnable threads, exit.
struct uthread* choose() {
  static struct uthread* th = uthreads;
  struct uthread* chosen;

  for (;;) {
    if (th == &uthreads[MAX_UTHREADS]) {
      th = uthreads;
    }

    if (th->state == RUNNABLE) {
       chosen = th;
       th++;
       return chosen;
    }

    th++;
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

int num_runnable() {
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
  if (num_runnable() == 1) { // exit if this is the last thread
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


// // returns a RUNNABLE thread.
// uthread* schedule() {
//   uthread* next;


// }

// void uthread_yield() {
//   uthread* th = this_uthread();
//   th->state = RUNNABLE;
//   uthread* next = schedule();
// }