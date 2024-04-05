#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define TAMBUFFER 10

pthread_mutex_t entrada;
pthread_mutex_t bloquear;

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
    pthread_t tid[2];

    srand(getpid());

    buffer_init(&buffer);
    pthread_mutex_init(&entrada, NULL);
    pthread_mutex_init(&bloquear, NULL);
    pthread_mutex_lock(&bloquear);

    pthread_create(&tid[0], NULL, productor, NULL);
    pthread_create(&tid[0], NULL, consumidor, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    pthread_mutex_destroy(&entrada);
    pthread_mutex_destroy(&bloquear);
}

void enter_monitor()
{
    pthread_mutex_lock(&entrada);
}

void leave_monitor()
{
    pthread_mutex_unlock(&entrada);
}

void cwait()
{
    bloqueados++;
    pthread_mutex_unlock(&entrada);
    pthread_mutex_lock(&bloquear);
    pthread_mutex_lock(&entrada);
    bloqueados--;
}

void cnotify()
{
    if (bloqueados > 0)
        pthread_mutex_unlock(&bloquear);
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
