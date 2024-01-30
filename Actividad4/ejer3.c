#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    int p = fork();

    if (!p) 
        sleep(20);
    else 
        sleep(1);
}

