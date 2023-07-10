#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/uthread.h"
#include "user/user.h"

void th1() {
  printf("1\n");
  uthread_yield();
  printf("1\n");
  uthread_yield();
  printf("1\n");
  uthread_yield();
  printf("1\n");
  uthread_exit();
}


void tha() {
  printf("a\n");
  uthread_yield();
  printf("a\n");
  uthread_yield();
  printf("a\n");
  uthread_yield();
  printf("a\n");
  uthread_exit();
}

void th2() {
  printf("2\n");
  uthread_yield();
  printf("2\n");
  uthread_yield();
  printf("2\n");
  uthread_yield();
  printf("2\n");
  uthread_exit();
}

void th3() {
  printf("3\n");
  uthread_yield();
  printf("3\n");
  uthread_yield();
  printf("3\n");
  uthread_yield();
  printf("3\n");
  uthread_exit();
}

int main(int argc, char** argv) {
  write(1, "hello world\n", 12);

  uthread_create(th1, HIGH);
  uthread_create(tha, HIGH);
  uthread_create(th2, MEDIUM);
  uthread_create(th3, LOW);
  uthread_start_all();
  printf("error!");
  exit(0);
}