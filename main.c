#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    char buf[100];

    //Test for correct arguments (0)
    if (argc > 1) {
        printf("ERROR: Too many arguments\n");
        return -1;
    }

    //read input file
    int k;
    int m;
    int n;
    int* status;

    scanf("%d", &k);

    //create an array of k integers in shared memory
    key_t qKey = ftok("./", 'j');

    sprintf(buf, "*** MAIN: shared memory key = %d\n", qKey);
    write(1, buf, strlen(buf));

    int q_shm_id = shmget(qKey, sizeof(int) * k, IPC_CREAT | 0666);

    sprintf(buf, "*** MAIN: shared memory created\n");
    write(1, buf, strlen(buf));

    int *a = (int*) shmat(q_shm_id, NULL, 0);

    sprintf(buf, "*** MAIN: shared memory attached and is ready to use\n");
    write(1, buf, strlen(buf));

    for (int i = 0; i < k; i++) {
        scanf("%d", &a[i]);
    }

    sprintf(buf, "Input array for qsort has %d elements:\n", k);
    write(1, buf, strlen(buf));

    sprintf(buf, "    ");
    int j = strlen(buf);
    for (int i= 0; i < k; i++) {
        sprintf(&buf[j], "%d ", a[i]);
        j = strlen(buf);
    }
    sprintf(&buf[j], "\n");
    write(1, buf, strlen(buf));

    scanf("%d", &m);
    int* x = malloc(sizeof(int) * m);
    for (int i = 0; i < m; i++) {
        scanf("%d", &x[i]);
    }

    sprintf(buf, "Input array x[] for merge has %d elements:\n", m);
    write(1, buf, strlen(buf));

    sprintf(buf, "    ");
    j = strlen(buf);
    for (int i = 0; i < m; i++) {
        sprintf(&buf[j],"%d ", x[i]);
        j = strlen(buf);
    }

    sprintf(&buf[j], "\n");
    write(1, buf, strlen(buf));

    scanf("%d", &n);
    int* y = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &y[i]);
    }

    sprintf(buf, "Input array y[] for merge has %d elements:\n", n);
    write(1, buf, strlen(buf));

    sprintf(buf, "    ");
    j = strlen(buf);
    for (int i = 0; i < n; i++) {
        sprintf(&buf[j], "%d ", y[i]);
        j = strlen(buf);
    }

    sprintf(&buf[j], "\n");
    write(1, buf, strlen(buf));

    //execvp qsort
    pid_t qChild = fork();
    if (qChild < 0) {
        perror("fork failed!\n");
        exit(1);
    } else if (qChild == 0) {
        //Child Logic
        char prog[] = {"./qsort"};
        char l[10];
        char r[10];
        char size[10];
        char shmKey[10];

        sprintf(l, "%d", 0);
        sprintf(r, "%d", k - 1);
        sprintf(size, "%d", k);
        sprintf(shmKey, "%d", qKey);

        char* args[] = {prog, l, r, size, shmKey, '\0'};

        if (execvp(prog, args) < 0) {
            perror("*** MAIN: execvp() failed! ");
            exit(1);
        }
    }


    free(y);
    free(x);

    waitpid(qChild, status, 0);
    //detach and remove shared memory
    shmdt(a);

    sprintf(buf, "*** MAIN: shared memory successfully detached\n");
    write(1, buf, strlen(buf));

    if (shmctl(q_shm_id, IPC_RMID, NULL) == -1) {
        sprintf(buf, "*** MAIN: shared memory unable to be removed!\n");
        write(1, buf, strlen(buf));
        exit(1);
    }

    sprintf(buf, "*** MAIN: shared memory successfully removed\n");
    write(1, buf, strlen(buf));

    return 0;
}
