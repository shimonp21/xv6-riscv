#include "kernel/types.h"
#include <user/user.h>
#include "ustack.h"

int main(int argc, char *argv[]) {
  int i;

  printf("%d\n", sizeof(int));

  char* c1 = ustack_malloc(6);
  if ((uint64)c1 == -1) {
    printf("err!\n");
    exit(1);
  }
  strcpy(c1, "hello");

  char* c2 = ustack_malloc(7);
  if ((uint64)c1 == -1) {
    printf("err!\n");
    exit(1);
  }
  strcpy(c2, "bye");

  ustack_free();
  c2 = ustack_malloc(7);
  if ((uint64)c1 == -1) {
    printf("err!\n");
    exit(1);
  }
  strcpy(c2, "bye");

  char* c3 = ustack_malloc(8);
  if ((uint64)c1 == -1) {
    printf("err!\n");
    exit(1);
  }
  strcpy(c3, "other");

  printf("%s %s %s\n", c1, c2, c3);

  char* c4 = ustack_malloc(0);
  if ((uint64)c1 == -1) {
    printf("err!\n");
    exit(1);
  }
  c4 = c4;

  char* base = c4 - 112;

  for (i = 0; i < 128; i += 1) {
    printf("%x ", *(base + i));
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("%d\n",ustack_free());
  printf("%d\n",ustack_free());
  printf("%d\n",ustack_free());
  printf("%d\n",ustack_free());
  printf("%d\n",ustack_free());

  exit(0);
}
