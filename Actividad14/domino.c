#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define JUGADORES 4

pthread_mutex_t semaforos[JUGADORES];

void *jugar(void *args)
{
    int n = *((int *)args);
    pthread_mutex_lock(&semaforos[n]);
    // Hace su jugada
    printf("Jugador %d juega\n", n);
    //
    pthread_mutex_unlock(&semaforos[(n + 1) % JUGADORES]);
}

int main(void)
{
    pthread_t tid[JUGADORES];
    int args[JUGADORES];
    int i;

    // Hacemos un lock en todos los semaforos menos el primero
    for (i = 0; i < JUGADORES; i++)
    {
        pthread_mutex_init(&semaforos[i], NULL);
        // Solo 0 empieza con su semaforo en 1 para que sea el primero en empezar
        if (i != 0)
            pthread_mutex_lock(&semaforos[i]);
    }

    for (i = 0; i < JUGADORES; i++)
    {
        args[i] = i;
        pthread_create(&tid[0], NULL, jugar, &args[i]);
    }

    for (i = 0; i < JUGADORES; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
