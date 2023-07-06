#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  printf("hi!\n");

  set_ps_priority(1);
  printf("hi!\n");

  set_ps_priority(10);
  printf("hi!\n");

  exit(0);
}