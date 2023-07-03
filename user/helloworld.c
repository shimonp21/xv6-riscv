#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_num_reverse(int num) {
  int n = num;
  char ch = 0;

  if (n == 0) {
    write(1, "0", 1);
    return;
  }

  while (n > 0) {
    ch = (n % 10) + '0';
    write(1, &ch, 1);
    n = n / 10;
  }
}

int
main(int argc, char *argv[]) {
  char* str = "Hello World xv6\n";
  write(1, str, strlen(str));

  __builtin_trap();
  
  print_num_reverse(1542);
  write(1, "\n", 1);

  exit(0);
}