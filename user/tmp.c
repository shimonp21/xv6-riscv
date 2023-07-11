#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void* func() {
  printf("kt1!\n");
  sleep(10);
  printf("kt2\n");
  kthread_exit(0);
}

int
main(int argc, char *argv[])
{
  int tid;
  void* stack;

  stack = malloc(4000);
  if (stack == 0) {
    printf("malloc failed\n");
    exit(1);
  }

  tid = kthread_create(func, stack, 4000);
  if (tid < 0) {
    printf("kthread create failed\n");
    exit(1);
  }

  sleep(30);
  printf("m!\n");
  kthread_exit(0);
}
