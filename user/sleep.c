#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  printf("hello\n");
  for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
    sleep(10);
  }

  printf("bye\n");


  
  exit(0);
}
