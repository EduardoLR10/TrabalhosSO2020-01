/*---------------------------------------------------------------------------
  Integrantes:
    - Giordano Suffert Monteiro - 170011160;
    - Eduardo Lemos Rocha - 170009157;
    - Otho Teixeira Komatsu - 170020142
    - Pedro Lucas Pinto Andrade - 160038316;

  Versão do SO: Ubuntu 16.04
  Versões do gcc testadas: 5.4.0 e 9.3.0
---------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

typedef struct mensagem{
    long pid;
    char msg[50];
} t_mensagem;

void alarm_handler(int signum){}

void P2(int idfila)
{
    t_mensagem msg_env;
    int i;
    char msg_temp[50];
    int pausa;

    msg_env.pid = getpid();

    for(i=0; i<10; i++)
    {
        // Espera de 1 a 4 segundos
        pausa = rand() % 4;
        pausa++;

        // Processo aguarda para enviar mensagem
        if(sleep(pausa) > 0)
        {
            printf("Sleep interrompido.");
            exit(1);
        }

        // Formata mensagem
        sprintf(msg_temp, "pid=%ld numero=%d", msg_env.pid, i+1);
        strcpy(msg_env.msg, msg_temp);

        // Envia mensagem
        msgsnd(idfila, &msg_env, sizeof(msg_env), 0);

        // Mostra mensagem na tela
        printf("mensagem %s enviada\n", msg_env.msg);
    }
}


void P1(int idfila)
{
    t_mensagem msg_rcv;
    int i, bytes_read;

    if(siginterrupt(SIGALRM, 1) == -1)
    {
        printf("Erro de sinal inválido\n");
    }

    for(i= 0; i < 10; i++)
    {
        // Se nao receber em 2 segundos, TIMEOUT, envia SIGALRM - para de esperar mensagem
        alarm(2);

        // Se leu mensagem e não deu timeout
        if(msgrcv(idfila, &msg_rcv, sizeof(msg_rcv), 0, 0) != -1)
        {
            // Desarma alarme
            alarm(0);
            // Imprime conteudo da mensagem
            printf("mensagem %s recebida\n", msg_rcv.msg);
        } else {
            if(errno == EINTR){
                printf("ocorreu timeout - nao recebi mensagem em 2 s\n");
            } else {
                printf("Erro ao receber mensagem\n");
            }
        }
    }
}


int main(){
    int pid, idfila;
    time_t t;

    // Define seed para o gerador aleatorio
    srand((unsigned) time(&t));

    // Indica o tratamento para o sinal de alarme
    signal(SIGALRM, alarm_handler);


    // Cria a fila
    if((idfila = msgget(0x38316, IPC_CREAT|0x1ff)) < 0)
    {
        printf("Erro ao criar a fila de mensagens\n");
        exit(1);
    }

    // Cria processo P2 (filho)
    if((pid = fork()) < 0)
    {
        printf("Erro ao criar processo P2\n");
        exit(1);
    }

    if(pid == 0)
    {
        // Executa filho
        P2(idfila);
    }
    else
    {
        // Executa pai
        P1(idfila);
    }

    // Processo pai P1 remove a fila
    if(pid != 0)
    {
        msgctl(idfila, IPC_RMID, 0);
    }

    return 0;
}
