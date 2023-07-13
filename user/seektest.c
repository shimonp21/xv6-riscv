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
  int fd = open("hello.txt", O_RDONLY);
  char arr[100] = {0};
  int r = 0;
  
  if (fd == -1) {
    printf("open failed!");
    exit(1);
  }

  zero_arr(arr);
  r = read(fd, arr, 5);  
  if (r != 5) {
    printf("failed to read file!");
    exit(1);
  }
  printf("%s", arr);

  zero_arr(arr);
  r = read(fd, arr, 5);  
  if (r != 5) {
    printf("failed to read file!");
    exit(1);
  }
  printf("%s", arr);

  r = seek(fd, 2, SEEK_SET);
  if (r != 0) {
    printf("seek failed!");
    exit(1);
  }

  zero_arr(arr);
  r = read(fd, arr, 5);  
  if (r != 5) {
    printf("failed to read file!");
    exit(1);
  }
  printf("%s", arr);

  seek(fd, -3, SEEK_CUR);

  zero_arr(arr);
  r = read(fd, arr, 5);  
  if (r != 5) {
    printf("failed to read file!");
    exit(1);
  }
  printf("%s", arr);


  close(fd);
  exit(0);
}
