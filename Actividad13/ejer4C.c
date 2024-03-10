#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>

sem_t *sem;

void print(char *s)
{
    usleep(rand() % 1000);
    printf("%s\n", s);
    usleep(rand() % 1000);
}

void P()
{
    // Se hace un wait para asegurar que A y B se hacen de forma atomica
    sem_wait(sem);

    print("A");
    usleep(rand() % 100000);

    print("B");
    usleep(rand() % 100000);

    // Ya que se imprimen A y B, los otros procesos pueden continuar
    sem_post(sem);

    print("C");

    sem_close(sem);
    sem_unlink("/sem");
    exit(0);
}

void Q()
{
    sem_wait(sem);
    print("D");
    usleep(rand() % 100000);
    sem_post(sem);

    sem_wait(sem);
    print("E");
    usleep(rand() % 100000);
    sem_post(sem);

    sem_close(sem);
    sem_unlink("/sem");
    exit(0);
}

int main()
{
    srand(getpid());
    sem = sem_open("/sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);

    int p;

    p = fork();
    if (p == 0)
        P();

    p = fork();
    if (p == 0)
        Q();

    wait(NULL);
    wait(NULL);

    sem_close(sem);
    sem_unlink("/sem");
}
