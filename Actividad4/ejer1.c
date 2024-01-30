#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    int i;
    int p;

    p = fork();

    if (p) {
        while(1) {
            printf("Soy el padre\n");
            sleep(2);
        }
    }
    else {
        sleep(3);
        kill(getppid(), SIGKILL);
        execlp("ps", "ps", NULL);
    }
}

