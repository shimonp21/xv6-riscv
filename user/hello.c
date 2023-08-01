#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  char* str = "hello world and other things";
  
  printf("main=%p\n", main);
  printf("exit=%p\n", exit);
  printf("str=%p\n", str);

  write(1, str, 17);

  exit(0);
}
