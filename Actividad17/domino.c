#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/wait.h>

#define PLAYERS 4

#define FLAGS 0
#define MAXMSG 10
#define MSGSIZE 128
#define CURMSGS 0

mqd_t buzones[PLAYERS];	// Buzón de mensajes
struct mq_attr attr = {FLAGS,MAXMSG,MSGSIZE,CURMSGS};
int priority=1;

void jugada(int i)
{
    char msg[]="NADA XDXD";

    for(int n = 10; n > 0; n--)
    {
        mq_receive(buzones[i],msg,attr.mq_msgsize,&priority);

        // Hace su jugada
        printf("Inicia jugada  jugador %d",i);
        fflush(stdout);
        usleep(rand()%2000000);
        printf("-%d Termina su jugada\n",i);
        fflush(stdout);

        // Salida
        mq_send(buzones[(i + 1) % PLAYERS],msg,strlen(msg)+1,0);
    }
    exit(0);
}


int main()
{
    int p;
    int i;
    char msg[] = "NADA";
    char buzon_nombre[12];

    srand(getpid());
	
    for(i=0;i<PLAYERS;i++)
    {
        sprintf(buzon_nombre,"/buzon_%d",i);
        mq_unlink(buzon_nombre);

        // Crear buzón de mensajes 
	    buzones[i] = mq_open(buzon_nombre,O_RDWR | O_CREAT, 0600, &attr);
	    if(buzones[i]==-1)
		    fprintf(stderr,"Error al crear la cola de mensajes\n");
    }
 
    // Inicia el jugador 0
    mq_send(buzones[0],msg,attr.mq_msgsize,0);


    for(i=0;i<PLAYERS;i++)
    {
        p = fork();
        if(p==0)
            jugada(i);
    }

    for(i=0;i<PLAYERS;i++)
        wait(NULL);

    for(i=0;i<PLAYERS;i++)
    {
        mq_close(buzones[i]);
        sprintf(buzon_nombre,"/buzon_%d",i);
        mq_unlink(buzon_nombre);
    }
}
