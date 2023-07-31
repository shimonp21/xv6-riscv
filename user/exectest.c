

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void* func() {
  int i;
  for (i = 0; i < 20; i++) {
    sleep(1);
    printf("kt %d!\n", 1);
  }
  kthread_exit(0);
}

int
main(int argc, char *argv[])
{
  int tid;
  void* stack;
  char* path = "sleeper";
  // char* arg = "hello";
  char* argvvvv[] = {path, 0};

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

  sleep(5);

  exec(path, argvvvv);
  printf("press F to pay respects");
  sleep(1000);
  exit(1);
}
