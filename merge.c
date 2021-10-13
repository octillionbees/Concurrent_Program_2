#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/* ----------------------------------------------------------- */
/* NAME : Jade Wang                         User ID: jbwang    */
/* DUE DATE : 10/13/2021                                       */
/* PROGRAM ASSIGNMENT 2                                        */
/* FILE NAME : merge.c                                         */
/* PROGRAM PURPOSE :                                           */
/*    Will be called by main.c in an execvp system call with 5 */
/*    command line arguments. The program merges two arrays in */
/*    shared memory*/
/* ----------------------------------------------------------- */

int binarySearch(int* arr, int l, int r, int x);

/* ----------------------------------------------------------- */
/* FUNCTION  main :                                            */
/*     Main function, merges two arrays in shared memory into a*/
/*     third array in shared memory.                           */
/* PARAMETER USAGE :                                           */
/*    argc : number of command line arguments passed in        */
/*    argv : array of command line arguments : {               */
/*           key for first shared memory array,                */
/*           key for second shared memory array,               */
/*           size of first shared memory array in elements     */
/*           size of second shared memory array in elements    */
/*           key for third shared memory array                 */
/*           }                                                 */
/* FUNCTION CALLED :                                           */
/*    binarySearch()                                           */
/* ----------------------------------------------------------- */
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
    int m = atoi(argv[3]);
    int n = atoi(argv[4]);
    int mergeKey = atoi(argv[5]);

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
        pid_t forkPid = fork();
        if (forkPid == -1) {
            char errorMsg[100];
            sprintf(errorMsg, "      $$$ M-PROC(%d): fork(), i = %d, m = %d, n = %d process failed", pid, i, m , n);
            perror(errorMsg);
            exit(1);
        } else if (forkPid == 0) {
            //child logic
            pid_t childPid = getpid();

            int ind = i;
            if (ind > (m - 1)) {
                //logic for y[ind]
                ind -= m;
                int val = y[ind];
                sprintf(buf, "      $$$ M-PROC(%d): handling y[%d] = %d\n", childPid, ind, val);
                write(1, buf, strlen(buf));

                //edge cases
                int mergeInd;
                if (val < x[0]) {
                    sprintf(buf, "      $$$ M-PROC(%d): y[%d] = %d is found to be smaller than x[0] = %d\n", childPid, ind, val, x[0]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind;
                } else if (val > x[m-1]) {
                    sprintf(buf, "      $$$ M-PROC (%d): y[%d] = %d is found to be larger than x[%d] = %d\n", childPid, ind, val, m-1, x[m-1]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind + m;
                } else { //x[0] < y[ind] < x[m-1]
                    //modified binary search
                    int k = binarySearch(x, 0, m - 1, val);
                    if (k == -1) {
                        sprintf(buf, "      $$$ M-PROC(%d): ERROR: Binary Search Failed!\n");
                        write(1, buf, strlen(buf));
                        exit(1);
                    }
                    sprintf(buf, "      $$$ M-PROC(%d): y[%d] = %d is found between x[%d] = %d and x[%d] = %d\n", childPid, ind, val, k-1, x[k-1], k, x[k]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind + k;
                }

                sprintf(buf, "      $$$ M-PROC(%d): about to write y[%d] = %d into position %d of the output array\n", childPid, ind, val, mergeInd);
                write(1, buf, strlen(buf));
                mergeArr[mergeInd] = val;
            } else { //ind < m - 1
                //logic for x[ind]
                int val = x[ind];

                sprintf(buf, "      $$$ M-PROC(%d): handling x[%d] = %d\n", childPid, ind, val);
                write(1, buf, strlen(buf));

                //edge cases
                int mergeInd;
                if (val < y[0]) {
                    sprintf(buf, "      $$$ M-PROC(%d): x[%d] = %d is found to be smaller than y[0] = %d\n", childPid, ind, val, y[0]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind;
                } else if (val > y[n - 1]) {
                    sprintf(buf, "      $$$ M-PROC(%d): x[%d] = %d is found to be larger than y[%d] = %d\n", childPid, ind, val, n - 1, y[n-1]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind + n;
                } else { //y[0] < x[ind] < y[n - 1]
                    int k = binarySearch(y, 0, n - 1, val);
                    if (k == -1) {
                        sprintf(buf, "      $$$ M-PROC(%d): ERROR: Binary Search Failed!\n");
                        write(1, buf, strlen(buf));
                        exit(1);
                    }
                    sprintf(buf, "      $$$ M-PROC(%d): x[%d] = %d is found between y[%d] = %d and y[%d] = %d\n", childPid, ind, val, k - 1, y[k-1], k, y[k]);
                    write(1, buf, strlen(buf));
                    mergeInd = ind + k;
                }

                sprintf(buf, "      $$$ M-PROC(%d): about to write x[%d] = %d into position %d of the output array\n", childPid, ind, val, mergeInd);
                write(1, buf, strlen(buf));
                mergeArr[mergeInd] = val;
            }
            //finish child process
            exit(0);
        }
        //mergeArr is sorted now

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

/* ----------------------------------------------------------- */
/* FUNCTION  binarySearch :                                    */
/*     calculates the point in a sorted array where a given    */
/*     value comes BEFORE using a modified binary search       */
/*     algorithm.                                              */
/* PARAMETER USAGE :                                           */
/*    int* arr : pointer to sorted array to be searched        */
/*    int l : index of left-most bound to search from          */
/*    int r : index of right-most bound to search from         */
/*    int x : value to conduct search with                     */
/* FUNCTION CALLED :                                           */
/*    binarySeach() (recursively)                              */
/* ----------------------------------------------------------- */
int binarySearch(int* arr, int l, int r, int x) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] > x && arr[mid - 1] < x) {
            return mid;
        }

        if (arr[mid - 1] > x) {
            return binarySearch(arr, l, mid - 1, x);
        } else { //arr[mid] < x
            return binarySearch(arr, mid + 1, r, x);
        }
    }
    return -1;
}