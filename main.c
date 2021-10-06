#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    //Test for correct arguments (0)
    if (argc > 1) {
        printf("ERROR: Too many arguments\n");
        return -1;
    }

    char* input;
    scanf("%ms", input);

    printf("Argc = %d\n", argc);
    printf("Input: %s\n", input);
    free(input);


    return 0;
}
