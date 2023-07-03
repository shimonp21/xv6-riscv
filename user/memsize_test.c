#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  printf("%p\n", memsize());
  char* ch = malloc(20000);
  printf("%p\n", memsize());
  free(ch);
  printf("%p\n", memsize());

  exit(0);
}