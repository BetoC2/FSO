#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define SIZE 4000

#define INICIAL 900000000
#define FINAL 1000000000

#define N_THREADS 4

int mat[SIZE][SIZE];

void initmat(int mat[][SIZE]);
void printnonzeroes(int mat[SIZE][SIZE]);
int isprime(int n);
void *paralelizado(void *args);

int main()
{
	long long start_ts;
	long long stop_ts;
	long long elapsed_time;
	long lElapsedTime;
	struct timeval ts;
	int i, j;
	pthread_t threads[N_THREADS];
	int args[N_THREADS];

	// Inicializa la matriz con números al azar
	initmat(mat);
	gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	// Eliminar de la matriz todos los números que no son primos
	// Esta es la parte que hay que paralelizar
	for (i = 0; i < N_THREADS; i++)
	{
		args[i] = i;
		pthread_create(&threads[i], NULL, paralelizado, &args[i]);
	}

	for (i = 0; i < N_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	// Hasta aquí termina lo que se tiene que hacer en paralelo
	gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	printnonzeroes(mat);
	printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);
}

void *paralelizado(void *args)
{
	int n = *((int *)args);
	for (int i = n; i < SIZE; i += N_THREADS)
		for (int j = 0; j < SIZE; j++)
			if (!isprime(mat[i][j]))
				mat[i][j] = 0;
}

void initmat(int mat[][SIZE])
{
	int i, j;

	srand(getpid());

	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			mat[i][j] = INICIAL + rand() % (FINAL - INICIAL);
}

void printnonzeroes(int mat[SIZE][SIZE])
{
	int i, j;

	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			if (mat[i][j] != 0)
				printf("%d\n", mat[i][j]);
}

int isprime(int n)
{
	int d = 3;
	int prime = n == 2;
	int limit = sqrt(n);

	if (n > 2 && n % 2 != 0)
	{
		while (d <= limit && n % d)
			d += 2;
		prime = d > limit;
	}
	return (prime);
}