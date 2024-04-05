#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TAMBUFFER 10

sem_t *monitor;
sem_t *monitor_block;
int bloqueados = 0;

typedef struct
{
    int in;                // Posición del siguiente elemento por entrar al buffer
    int out;               // Posición del siguiente elemento a retirar del buffer
    int buffer[TAMBUFFER]; // Buffer circular
} BUFFER;

void enter_monitor();
void leave_monitor();
void cwait();
void cnotify();

void buffer_init(BUFFER *bf);
void buffer_put(BUFFER *b, int data);
int buffer_get(BUFFER *b);

void *productor(void *args);
void *consumidor(void *args);
;

BUFFER buffer;
int elementos = 0;

int main()
{
    monitor = sem_open("/monitor", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    monitor_block = sem_open("/monitor_block", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);

    pthread_t tid[2];

    srand(getpid());

    buffer_init(&buffer);

    pthread_create(&tid[0], NULL, productor, NULL);
    pthread_create(&tid[0], NULL, consumidor, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    sem_close(monitor);
    sem_close(monitor_block);

    sem_destroy(monitor);
    sem_destroy(monitor_block);
}

void enter_monitor()
{
    // Usa los semáforos necesarios para permitir
    // que solo un hilo entre el monitor
    sem_wait(monitor);
}

void leave_monitor()
{
    // Usa los semáforos necesarios para que
    // el hilo libere el monitor
    sem_post(monitor);
}

void cwait()
{
    // Usa los semáforos necesarios para que el hilo
    // que lo ejecute se bloquée en una cola de condición
    bloqueados++;
    sem_post(monitor);
    sem_wait(monitor_block);
    sem_wait(monitor);
    bloqueados--;
}

void cnotify()
{
    // Usa los semáforos necesarios para que el hilo
    // que se libere un hilo que está en la cola de
    // condición
    if (bloqueados)
        sem_post(monitor_block);
}

void buffer_init(BUFFER *bf)
{
    bf->in = 0;
    bf->out = 0;
}

void buffer_put(BUFFER *bf, int e)
{
    enter_monitor();

    while (elementos == TAMBUFFER)
        cwait();

    bf->buffer[bf->in] = e;
    bf->in++;
    bf->in = bf->in % TAMBUFFER;

    elementos++;

    cnotify();

    leave_monitor();
}

int buffer_get(BUFFER *bf)
{
    int retval;

    enter_monitor();

    while (elementos == 0)
        cwait();

    retval = bf->buffer[bf->out];
    bf->out++;
    bf->out = bf->out % TAMBUFFER;

    elementos--;

    cnotify();

    leave_monitor();

    return (retval);
}

void *productor(void *args)
{
    int e = 0;
    int n = 10;
    while (n--)
    {
        e = e + 1;
        printf("Productor produce %d\n", e);
        buffer_put(&buffer, e);
        usleep(rand() % 3000000);
    }
}

void *consumidor(void *args)
{
    int e;
    int n = 10;
    while (n--)
    {
        e = buffer_get(&buffer);
        printf("\t\t\tConsumidor consume %d\n", e);
        usleep(rand() % 3000000);
    }
}