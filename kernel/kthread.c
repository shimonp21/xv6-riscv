#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

extern void forkret(void);

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

void kthreadinit(struct proc *p)
{
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "kthread");
    kt->state = KT_UNUSED;
    kt->pcb = p;
    kt->killed = 0;

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

// p->lock must be held
int alloctid(struct proc* p) {
  int pid = p->nexttid;
  
  p->nexttid++;
  
  return pid;
}

// p->lock must be held.
struct kthread* allocthread(struct proc* p) {
  int i;
  struct kthread* th;

  for (th = p->kthread, i = 0; th < &p->kthread[NKT]; th++, i++) {
    acquire(&th->lock);
    if (th->state == KT_UNUSED) {
      goto found;
    } else {
      release(&th->lock);
    }
  }

  return 0;

  found:
  th->tid = alloctid(p);
  th->state = KT_USED;
  th->trapframe = get_kthread_trapframe(p, th);


  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&th->context, 0, sizeof(th->context));
  th->context.ra = (uint64)forkret;
  th->context.sp = th->kstack + PGSIZE;

  return th;
}

struct kthread *mykthread()
{
  return &myproc()->kthread[0];
}


void freethread(struct kthread* kt) {
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->tid = 0;
  kt->trapframe = 0;
  kt->state = KT_UNUSED;
}