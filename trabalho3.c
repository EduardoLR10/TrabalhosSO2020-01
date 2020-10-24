/*---------------------------------------------------------------------------
  Integrantes:
    - Giordano Suffert Monteiro - 170011160;
    - Eduardo Lemos Rocha - 170009157;
    - Otho Teixeira Komatsu - 170020142
    - Pedro Lucas Pinto Andrade - 160038316;

  Versão do SO: Ubuntu?
  Versão do gcc: usei 9.3.0
---------------------------------------------------------------------------*/
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>


typedef struct mensagem{
    long pid;
    char msg[50];
} t_mensagem;

void P2(){
    t_mensagem msg_env;
    int i;
    char msg_temp[50];
    int pausa;

    msg_env.pid = getpid();

    for(i=0; i<10; i++){
        //espera de 1 a 4 segundos
        pausa = rand() % 4;
        if(pausa == 0){
            pausa++;
        }
        sleep(pausa);

        //formata mensagem
        sprintf(msg_temp, "pid=%d numero=%d", msg_env.pid, i);
        strcpy(msg_env.msg, msg_temp);

        //envia mensagem
        msgsnd(idfila, &msg_env; sizeof(msg_env), 0);

        printf("mensagem %s enviada\n", msg_env.msg);
    }


}


void P1(){

}


int main(){
    int pid, idfila;
    time_t t;

    srand((unsigned) time(&t));


    //criacao da fila
    if((idfila = msgget(0x38316, IPC_CREAT|0x1ff)) < 0){
        printf("Erro ao criar a fila de mensagens\n");
        exit(1);
    }

    //criacao do processo P2 (filho)
    if((pid = fork()) < 0){
        printf("Erro ao criar processo P2\n");
        exit(1);
    }

    if(pid == 0){
        //processo filho P2
        P2();
    }
    else{
        //processo pai
        P1();
    }





    return 0;
}
