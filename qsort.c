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

int partition(int* a, int l, int r);
void swap(int* a, int i, int j);

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
    int k = atoi(argv[3]);
    int key = atoi(argv[4]);

    //attach shared memory
    int shm_id = shmget(key, sizeof(int) * k, 0666); //shm only created by main
    int* a = (int*) shmat(shm_id, NULL, 0); //attach shared memory to pointer a
    //partitioning algorithm

    partition(a, left, right);

    //wait

    //detach shared memory (don't remove, main will remove)

}

int partition(int* a, int l, int r){
    int p = a[l];
    int s = l;

    for (int i = l + 1; i <= r; i++) {
        if (a[i] < p) {
            s++;
            swap(a, s, i);
        }
    }
    swap(a, l, s);

    return s;
}

void swap(int* a, int i, int j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}