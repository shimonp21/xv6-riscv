#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void* func() {
  printf("kt!\n");
  sleep(100000);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int tid;
  void* stack;
  
  printf("%d\n", kthread_id());

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

  sleep(100000);
  exit(0);
}
