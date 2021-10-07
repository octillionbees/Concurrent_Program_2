//
// Created by Jade on 06-Oct-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[]) {
    char buf[100];
    pid_t pid = getpid();

    if (argc < 4) {
        sprintf(buf, "   ### Q-PROC(%d): ERROR: Too few command line arguments!\n", pid);
        write(1, buf, strlen(buf));
        exit(0);
    }

    int left = atoi(argv[1]);
    int right = atoi(argv[2]);
    int key = atoi(argv[3]);

    //attach shared memory

    //partitioning algorithm

    //wait, and exit

}