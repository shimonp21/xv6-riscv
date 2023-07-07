#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



void do_test(int id) {
  long long int i = 0;
  struct cfs_stat stat = {0};

  for (i = 0; i < 1000000000; i++) {
    if (i % 100000000 == 0) {
      sleep(10); // sleep 1 second (10 ticks)/
    }
  }

  get_cfs_stats(&stat);  

  sleep(id * 4); // jitter sleep

  printf("pid=%d, cfs_prio=%d, sleeptime=%d, runtime=%d, totaltime=%d, runnabletime=%d\n",
    getpid(),
    stat.cfs_priority,
    stat.sleeptime,
    stat.runtime,
    stat.totaltime,
    stat.runnabletime
  );

  exit(0);
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 3; i++) {
    int rv = fork();

    if (rv < 0) {
      printf("error!\n");
    }

    if (rv > 0) { // parent
      continue;
    }

    // child
    set_cfs_priority(i);
    do_test(i);
    exit(0);
  }

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  exit(0);
}
