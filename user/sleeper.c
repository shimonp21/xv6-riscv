
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    printf("sleeper starting\n");
    sleep(100);
    printf("sleeper done\n");
    exit(0);
}