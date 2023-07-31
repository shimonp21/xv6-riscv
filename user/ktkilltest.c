

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int func_ktid;

void* func() {
  int i;
  printf("[func] tid = %d\n", kthread_id());
  for (i = 0; i < 50; i++) {
    sleep(2);
    printf("kt %d!\n", i);
  }
  kthread_exit(17);
}

void* killer() {
  int rv;
  printf("[killer] tid = %d\n", kthread_id());
  sleep(20);
  rv = kthread_kill(func_ktid);
  if (rv != 0) {
    printf("ktkill failed\n");
  }

  kthread_exit(0);
}

int
main(int argc, char *argv[])
{
  int sleep_ktid;
  void* stack1;
  void* stack2;
  int status;

  printf("[main] tid = %d\n", kthread_id());

  stack1 = malloc(4000);
  stack2 = malloc(4000);
  if (stack1 == 0  || stack2 == 0) {
    printf("malloc failed\n");
    exit(1);
  }

  func_ktid = kthread_create(func, stack1, 4000);
  if (func_ktid < 0) {
    printf("kthread create failed\n");
    exit(1);
  }
  sleep(1);

  sleep_ktid = kthread_create(killer, stack2, 4000);
  if (sleep_ktid < 0) {
    printf("kthread create failed\n");
    exit(1);
  }

  kthread_join(func_ktid, &status);
  printf("[main] done. exit status = %d\n", status);
  exit(0);
}
