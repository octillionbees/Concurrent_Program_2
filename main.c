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

    sprintf(buf, "*** MAIN: qSort shared memory key = %d\n", qKey);
    write(1, buf, strlen(buf));

    int q_shm_id = shmget(qKey, sizeof(int) * k, IPC_CREAT | 0666);

    sprintf(buf, "*** MAIN: qSort shared memory created\n");
    write(1, buf, strlen(buf));

    int *a = (int*) shmat(q_shm_id, NULL, 0);

    sprintf(buf, "*** MAIN: qSort shared memory attached and is ready to use\n");
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

    //create an array of m integers in shared memory
    key_t mKey = ftok("./", 'a');
    sprintf(buf, "*** MAIN: first merge shared memory key = %d\n", mKey);
    write(1, buf, strlen(buf));
    int m_shm_id = shmget(mKey, sizeof(int) * m, IPC_CREAT | 0666);
    sprintf(buf, "*** MAIN: first merge shared memory created\n");
    write(1, buf, strlen(buf));
    int *x = (int*) shmat(m_shm_id, NULL, 0);
    sprintf(buf, "*** MAIN: first merge shared memory attached and is ready to use\n");
    write(1, buf, strlen(buf));

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

    //create an array of n integers in shared memory
    key_t nKey = ftok("./", 'd');
    sprintf(buf, "*** MAIN: second merge shared memory key = %d\n", nKey);
    write(1, buf, strlen(buf));
    int n_shm_id = shmget(nKey, sizeof(int)*n, IPC_CREAT | 0666);
    sprintf(buf, "*** MAIN: second merge shared memory created\n");
    write(1, buf, strlen(buf));
    int *y = (int*) shmat(n_shm_id, NULL, 0);
    sprintf(buf, "*** MAIN: second merge shared memory attached and is ready to use\n");
    write(1, buf, strlen(buf));

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

    //Last Shared Memory needed
    key_t mergeKey = ftok("./", 'e');
    sprintf(buf, "*** MAIN: third merge shared memory key = %d\n", mergeKey);


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

    waitpid(qChild, status, 0);

    //Cleanup Time!
    //detach and remove shared memory
    shmdt(a);

    sprintf(buf, "*** MAIN: qSort shared memory successfully detached\n");
    write(1, buf, strlen(buf));

    shmdt(x);

    sprintf(buf, "*** MAIN: first merge shared memory successfully detached\n");
    write(1, buf, strlen(buf));

    shmdt(y);

    sprintf(buf, "*** MAIN: second merge shared memory successfully detached\n");
    write(1, buf, strlen(buf));

    if (shmctl(q_shm_id, IPC_RMID, NULL) == -1) {
        perror("Unable to remove qSort shared memory");
        exit(1);
    }

    sprintf(buf, "*** MAIN: qSort shared memory successfully removed\n");
    write(1, buf, strlen(buf));

    if (shmctl(m_shm_id, IPC_RMID, NULL) == -1) {
        perror("Unable to remove first merge shared memory");
        exit(1);
    }

    sprintf(buf, "*** MAIN: first merge shared memory successfully removed\n");
    write(1, buf, strlen(buf));

    if (shmctl(n_shm_id, IPC_RMID, NULL) == -1) {
        perror("Unable to remove second merge shared memory");
        exit(1);
    }

    sprintf(buf, "*** MAIN: second merge shared memory successfully removed\n");
    write(1, buf, strlen(buf));

    return 0;
}
