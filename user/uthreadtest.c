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
  printf("hello world\n");

  uthread_create(th1, MEDIUM);
  uthread_create(th2, MEDIUM);
  uthread_create(th3, MEDIUM);
  uthread_start_all();
  printf("error!");
  exit(0);
}