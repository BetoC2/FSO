#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main(int argc, char *argv[]) {
    int nivel = atoi(argv[1]);
    printf("%d\n", 0);
    int i;
    for (i = 1; i <= nivel; i++) {
        fork();
        printf("%d\n", i);
    }
}