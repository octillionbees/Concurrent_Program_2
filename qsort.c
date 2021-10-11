//
// Created by Jade on 06-Oct-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
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

    sprintf(buf, "   ### Q-PROC(%d): entering with a[%d...%d]\n", pid, left, right);
    write(1, buf, strlen(buf));

    if (left < right) {
        //attach shared memory
        int shm_id = shmget(key, sizeof(int) * k, 0666); //shm only created by main
        int* a = (int*) shmat(shm_id, NULL, 0); //attach shared memory to pointer a


        sprintf(buf, "       ");
        int j = strlen(buf);
        for (int i = left; i <= right; i++) {
            sprintf(&buf[j], "%d ", a[i]);
            j = strlen(buf);
        }
        sprintf(&buf[j], "\n");
        write(1, buf, strlen(buf));

        //partitioning and sorting algorithm

        int m = partition(a, left, right);

        sprintf(buf, "   ### Q-PROC(%d): pivot element is a[%d] = %d\n", pid, m, a[m]);
        write(1, buf, strlen(buf));

        //fork 2 child processes, execvp to recursively run program
        pid_t pidChild1 = fork();
        if (pidChild1 < 0) {
            perror("fork failed!");
        } else if (pidChild1 == 0) {
            //child logic
            //construct execvp arguments
            char prog[] = {"./qsort"};
            char l[10];
            char r[10];
            char size[10];
            char shmKey[10];

            sprintf(l, "%d", left);
            sprintf(r, "%d", m-1);
            sprintf(size, "%d", k);
            sprintf(shmKey, "%d", key);

            char* args[] = {prog, l, r, size, shmKey, '\0'};

            if (execvp(prog, args) < 0) {
                perror("execvp failed!");
                exit(1);
            }

        }
        //parent logic
        pid_t pidChild2 = fork();
        if (pidChild2 < 0) {
            perror("fork failed!");
            exit(1);
        } else if (pidChild2 == 0) {
            //child logic
            char prog[] = {"./qsort"};
            char l[10];
            char r[10];
            char size[10];
            char shmKey[10];

            sprintf(l, "%d", m + 1);
            sprintf(r, "%d", right);
            sprintf(size, "%d", k);
            sprintf(shmKey, "%d", key);

            char* args[] = {prog, l, r, size, shmKey, '\0'};

            if (execvp(prog, args) < 0) {
                perror("execvp failed!");
                exit(1);
            }
        }
        //final parent logic
        //wait for both processes to finish;
        int* wStatus;

        waitpid(pidChild1, wStatus, 0);
        waitpid(pidChild2, wStatus, 0);


        sprintf(buf, "   ### Q-PROC(%d): section a[%d...%d] sorted\n", pid, left, right);
        write(1, buf, strlen(buf));

        sprintf(buf, "       ");
        j = strlen(buf);
        for (int i = left; i <= right; i++) {
            sprintf(&buf[j], "%d ", a[i]);
            j = strlen(buf);
        }
        sprintf(&buf[j], "\n");
        write(1, buf, strlen(buf));

        //detach shared memory (don't remove, main will remove)
        shmdt(a);

    }
    sprintf(buf,"   ### Q-PROC(%d): exits\n", pid);
    write(1, buf, strlen(buf));
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