#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

struct {
  struct spinlock lock;
  uint8 seed;

} rnd;

uint8 lfsr_char(uint8 lfsr) {
  uint8 bit;
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
  lfsr = (lfsr >> 1) | (bit << 7);
  return lfsr;
}

int
randomread(int user_dst, uint64 dst, int n) {
  int i;
  uint8 b;

  acquire(&rnd.lock);
  for (i = 0; i < n; i++) {
    b = lfsr_char(rnd.seed);
    rnd.seed = b;

    if (either_copyout(user_dst, dst + i, &b, 1) == -1) {
      break;
    }
  }

  release(&rnd.lock);
  return i;
}

int randomwrite(int user_src, uint64 src, int n) {
  uint8 b;
  
  if (n != 1) {
    return -1;
  }

  
  if (either_copyin(&b, user_src, src, 1) == -1) {
    return 0;
  }

  acquire(&rnd.lock);
  rnd.seed = b;
  release(&rnd.lock);

  return 1;
}

void devrandominit() {
  initlock(&rnd.lock, "rnd");
  rnd.seed = 0x2A;

  // connect read and write system calls
  // to randomread and randomwrite
  devsw[DEVRANDOM].read = randomread;
  devsw[DEVRANDOM].write = randomwrite;
}