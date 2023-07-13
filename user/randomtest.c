#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define ARR_SIZE 100

void zero_arr(char* arr) {
  for (int i = 0; i < ARR_SIZE; i++) {
    arr[i] = 0;
  }
}

int
main(int argc, char *argv[]) {
  int fd = open("/random", O_RDWR);
  uint8 buff[100] = {0};
  int r = 0;
  int i = 0;

  if (fd == -1) {
    printf("failed to open /random");
    exit(1);
  }

  r = read(fd, buff, 16);
  if (r != 16) {
    printf("read failed!\n");
    exit(1);
  }

  for (i = 0; i < 16; i++) {
    printf("%x ", buff[i]);
  }
  printf("\n");

  r = write(fd, buff + 6, 1);
  if (r != 1) {
    printf("write failed!\n");
    exit(1);
  }

  r = read(fd, buff, 16);
  if (r != 16) {
    printf("read failed!\n");
    exit(1);
  }
  
  for (i = 0; i < 16; i++) {
    printf("%x ", buff[i]);
  }
  printf("\n");

  close(fd);

  exit(0);
}
