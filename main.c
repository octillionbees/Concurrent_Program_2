#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    //Test for correct arguments (0)
    if (argc > 1) {
        printf("ERROR: Too many arguments\n");
        return -1;
    }

    //read input file
    int k;
    int m;
    int n;

    scanf("%d", &k);
    int* a = malloc(sizeof(int) * k); //create an array of k integers
    for (int i = 0; i < k; i++) {
        scanf("%d", &a[i]);
    }

    char buf[100];
    sprintf(buf, "Input array for qsort has %d elements:\n    ", k);
    write(1, buf, strlen(buf));

    for (int i= 0; i < k; i++) {
        sprintf(buf, "%d ", a[i]);
        write(1, buf, strlen(buf));
    }
    sprintf(buf, "\n");
    write(1, buf, strlen(buf));

    scanf("%d", &m);
    int* x = malloc(sizeof(int) * m);
    for (int i = 0; i < m; i++) {
        scanf("%d", &x[i]);
    }

    sprintf(buf, "Input array x[] for merge has %d elements:\n    ", m);
    write(1, buf, strlen(buf));

    for (int i = 0; i < m; i++) {
        sprintf(buf,"%d ", x[i]);
        write(1, buf, strlen(buf));
    }

    sprintf(buf, "\n");
    write(1, buf, strlen(buf));

    scanf("%d", &n);
    int* y = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &y[i]);
    }

    sprintf(buf, "Input array y[] for merge has %d elements:\n    ", n);
    write(1, buf, strlen(buf));

    for (int i = 0; i < n; i++) {
        sprintf(buf, "%d ", y[i]);
        write(1, buf, strlen(buf));
    }

    sprintf(buf, "\n");
    write(1, buf, strlen(buf));

    free(y);
    free(x);
    free(a);



    return 0;
}
