//
// Created by Jade on 11-Oct-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[]) {
    //arguments:
    //key for x
    //key for y
    //size of x (m)
    //size of y (n)
    //key for shared segment

    char buf[100];
    pid_t pid = getpid();


    if (argc < 6) {
        sprintf(buf, "      $$$ M-PROC(%d): ERROR: Too few command line arguments!\n", pid);
        write(1, buf, strlen(buf));
        exit(0);
    }

    int xKey = atoi(argv[1]);
    int yKey = atoi(argv[2]);
    int mergeKey = atoi(argv[3]);
    int m = atoi(argv[4]);
    int n = atoi(argv[5]);

    //attach shared memory segments
    int x_shm_id = shmget(xKey, sizeof(int) * m, 0666);
    int y_shm_id = shmget(yKey, sizeof(int) * n, 0666);
    int merge_shm_id = shmget(mergeKey, sizeof(int) * (m + n), 0666);
    int* x = shmat(x_shm_id, NULL, 0);
    int* y = shmat(y_shm_id, NULL, 0);
    int* mergeArr = shmat(merge_shm_id, NULL, 0);

    //create m + n child processes
    pid_t childPid;
    for (int i = 0; i < (m + n); i++) {
        forkPid = fork();
        if (forkPid < 0) {
            perror("      $$$ M-PROC: fork() process failed");
            exit(1);
        } else if (forkPid == 0) {
            //child logic
            pid_t childPid = getpid();

            int ind = i;
            if (ind > (m - 1)) ind -= m; //assign ind to be used for n

            //check edge cases first


            //finish child process
            exit(0);
        }
    }

    //wait all children exit
    int status;
    for (int i = 0; i < (m + n); i++) {
        wait(&status);
    }

    //dettach shared memory segments
    shmdt(x);
    shmdt(y);
    shmdt(mergeArr);
}