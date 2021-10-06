#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    //Test for correct arguments (0)
    if (argv > 0) {
        printf("ERROR: Too many arguments");
        return -1;
    }

    return 0;
}
