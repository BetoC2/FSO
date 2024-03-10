#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREADS 2
#define ITERS 100
#define BUFFER_SIZE 10

int m, n = 0;

pthread_mutex_t s;       // 1
pthread_mutex_t retraso; // 0
pthread_mutex_t lleno;   // 0

typedef struct
{
    char buffer[BUFFER_SIZE];
    int in;
    int out;
} BUFFER;

BUFFER buff;

#define END 0

char produce()
{
    static char c = -1;
    c++;
    return c % 26 + 65;
}

void buffer_put(BUFFER *b, char c)
{
    if (b->in == BUFFER_SIZE)
        b->in = 0;
    b->buffer[b->in++] = c;
}

char buffer_get(BUFFER *b)
{
    if (b->out == BUFFER_SIZE)
        b->out = 0;
    return (b->buffer[b->out++]);
}

void buffer_init(BUFFER *b)
{
    b->in = 0;
    b->out = 0;
}

void *productor(void *args)
{
    int iters = ITERS;
    char c;

    while (iters--)
    {
        c = produce();
        if (iters > 0)
            printf("Productor produce %c\n", c);

        pthread_mutex_lock(&s);

        if (iters > 0)
            buffer_put(&buff, c);
        else
            buffer_put(&buff, END);

        n++;
        m = n;
        if (n == 1)
            pthread_mutex_unlock(&retraso);

        pthread_mutex_unlock(&s);

        if (m == BUFFER_SIZE)
            pthread_mutex_lock(&lleno);

        usleep(rand() % 800000);
    }
}

void *consumidor(void *args)
{
    char c;

    pthread_mutex_lock(&retraso);

    while (1)
    {
        pthread_mutex_lock(&s);

        c = buffer_get(&buff);

        n--;
        m = n;

        if (n == BUFFER_SIZE - 1)
            pthread_mutex_unlock(&lleno);

        pthread_mutex_unlock(&s);

        if (c != END)
            printf("\t\t\tconsumidor consume %c\n", c);
        else
            break;

        if (m == 0)
            pthread_mutex_lock(&retraso);

        usleep(rand() % 1500000);
    }
}

int main()
{
    pthread_t tid[NTHREADS];
    int i;

    srand(getpid());

    buffer_init(&buff);

    pthread_mutex_init(&s, NULL);
    pthread_mutex_init(&retraso, NULL);
    pthread_mutex_init(&lleno, NULL);

    pthread_mutex_lock(&retraso); // Haz que retraso sea 0
    pthread_mutex_lock(&lleno);   // Lo mismo con lleno

    pthread_create(&tid[0], NULL, productor, NULL);
    pthread_create(&tid[1], NULL, consumidor, NULL);

    for (i = 0; i < NTHREADS; i++)
        pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&s);
    pthread_mutex_destroy(&retraso);
    pthread_mutex_destroy(&lleno);
}