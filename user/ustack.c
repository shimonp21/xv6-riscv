#include <kernel/types.h>
#include "ustack.h"
#include <kernel/riscv.h>
#include <user/user.h>

#define MAX_ALLOC 512

// stack structure:
// [ffff]
// ^
// | [start + size]
// | [sizeof(frame1) uint]
// | [frame1]
// | [sizeof(frame0) uint]
// | [frame0][start]
// v
// [0000]

char* start = 0;
uint size = 0;
uint capacity = 0;

// initializes 'start' and 'capacity.
// makes sure the first 'sbrk' is page-aligned
int ustack_init() {
  start = sbrk(0);
  if ((uint64)start == -1) {
    start = 0;
    return -1;
  }

  if ((uint64)start % PGSIZE != 0) {
    start = sbrk(PGSIZE - (uint64)start % PGSIZE);
    if ((uint64)start == -1) {
      start = 0;
      return -1;
    }  
  }
  
  start = sbrk(PGSIZE);
  printf("QQ: start=%d\n", start);
  if ((uint64)start == -1) {
    start = 0;
    return -1;
  }
  if ((uint64)start % PGSIZE != 0) {
    printf("unexpected sbrk result\n");
    start = 0;
    return -1;
  }

  capacity = PGSIZE;
  return 0;
}

void* ustack_malloc(uint len) {
  uint64 result;
  char* addr;

  if (len > MAX_ALLOC) {
    return (void*)-1;
  }

  if (start == 0) {
    if (ustack_init() == -1) {
      return (void*)-1;
    }
  }

  if (size + len + sizeof(uint) > capacity) {
    result = (uint64)sbrk(PGSIZE);
    if (result == -1) {
      return (void*)-1;
    }
    if (result != (uint64)(start + capacity)) { // sanity
      printf("unexpected answer from sbrk\n");
      return (void*)-1;
    }

    capacity += PGSIZE;
  }

  addr = start + size;
  *(uint*)(addr + len) = len;
  size += len + sizeof(uint);
  return addr;
}

int ustack_free() {
  int len;

  if (size == 0) {
    return -1;
  }

  len = *(uint*)(start + size - sizeof(uint));

  if (size - len - sizeof(uint) < 0) {
    printf("unexpected len!\n");
    return -1;
  }

  size -= len + sizeof(uint);

  if (capacity - size > PGSIZE) {
    sbrk(-PGSIZE);
    capacity -= PGSIZE;
  }

  return len;
}