#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

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
// returns with the thread lock held.
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
  push_off();
  struct cpu* c = mycpu();
  struct kthread* kt = c->kt;
  pop_off();
  return kt;
}


// Must be called with kt->locked acquired
void freethread(struct kthread* kt) {
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->tid = 0;
  kt->trapframe = 0;
  kt->state = KT_UNUSED;
}

int kthread_create(uint64 func, uint64 stack, uint32 stack_size) {
  struct kthread* kt;
  struct proc* p = myproc();
  int ktid;

  acquire(&p->lock);
  kt = allocthread(p);
  if (kt == 0) {
    release(&p->lock);
    return -1;
  }

  kt->trapframe->epc = func;
  kt->trapframe->sp = stack + stack_size;

  ktid = kt->tid;

  kt->state = KT_RUNNABLE;

  release(&kt->lock);
  release(&p->lock);

  return ktid;
}

// p->lock must be held
int num_running_runnable_sleeping(struct proc* p) {
  struct kthread* kt;
  int n = 0;

  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++) {
    acquire(&kt->lock);
    if (kt->state == KT_RUNNING || kt->state == KT_SLEEPING || 
      kt->state == KT_RUNNABLE) {
      n++;
    }
    release(&kt->lock);
  }

  return  n;
}

void kthread_exit(int status) {
  struct kthread* kt = mykthread();
  struct proc* p = myproc();

  acquire(&wait_lock);
  
  wakeup(kt);
  acquire(&p->lock);
  if (num_running_runnable_sleeping(p) == 1) {
    release(&p->lock);
    release(&wait_lock);
    exit(status);
  }

  acquire(&kt->lock);
  kt->state = KT_ZOMBIE;
  kt->xstate = status;

  release(&wait_lock);
  sched();
  panic("zombie kthread exit");
}

// kt->lock must be held
uint8 is_used(struct kthread* kt) {
  return kt->state != KT_UNUSED && 
    kt->state != KT_USED;
}

int kthread_join(int ktid, uint64 out_status) {
  struct kthread* self = mykthread();
  struct kthread* kt;
  struct proc* p = myproc();
  int status;

  acquire(&wait_lock);

  acquire(&p->lock);

  acquire(&self->lock);
  if (self->tid == ktid) {
    release(&self->lock);
    release(&p->lock);
    release(&wait_lock);
    return -1;
  }
  release(&self->lock);

  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++) {
    acquire(&kt->lock);
    if (is_used(kt) && kt->tid == ktid) {
      goto found;
    }
    release(&kt->lock);
  }

  release(&p->lock);
  release(&wait_lock);
  return -1; // given ktid was not found
  
  found:
  if (kt->state != KT_ZOMBIE) {
    release(&kt->lock);
    release(&p->lock);
    sleep(kt, &wait_lock); // sleep until the thread exits.
    acquire(&p->lock);
    acquire(&kt->lock);
  }

  status = kt->xstate;
  freethread(kt);

  if (out_status != 0 &&
    copyout(p->pagetable, out_status, (char*)&status, sizeof(status)) < 0) {
    return -1;
  }

  release(&kt->lock);
  release(&p->lock);
  release(&wait_lock);
  return 0;
}