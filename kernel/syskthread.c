#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_kthread_id() {
  return mykthread()->tid;
}

uint64
sys_kthread_create() {
  uint64 func;
  uint64 stack;
  int stack_size;

  argaddr(0, &func);
  argaddr(1, &stack);
  argint(2, &stack_size);

  return kthread_create(func, stack, stack_size);
}

uint64
sys_kthread_exit() {
  int status;

  argint(0, &status);

  return kthread_exit(status);
}