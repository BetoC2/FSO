#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    int p = fork();
    if (!p) {
        int p2 = fork();
        sleep(5);
        kill(p, SIGCHLD);
        kill(p2, SIGCHLD);
        return 0;
    }
    p = fork();
    if (!p) {
        p = fork();
        if (!p) {
            p = fork();
        } 
    }
    sleep(20);

}

