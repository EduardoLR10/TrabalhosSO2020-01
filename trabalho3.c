/*---------------------------------------------------------------------------
  Integrantes:
    - Giordano Suffert Monteiro - 170011160;
    - Eduardo Lemos Rocha - 170009157;
    - Otho Teixeira Komatsu - 170020142
    - Pedro Lucas Pinto Andrade - 160038316;

  Versão do SO: Ubuntu?
  Versão do gcc: usei 9.3.0
---------------------------------------------------------------------------*/
// #include<errno.h>
//mensagens
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
//tratamento de sinais
#include<signal.h>
//standards
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

//para definir a seed do gerador aleatório, não diz que pode usar na especificação
#include<time.h>

//para colocar a string na mensagem, não diz na especificação que pode usar, mas é como é feito no exemplo do slide (não vejo qual problema pode ter também)
#include<string.h>


typedef struct mensagem{
    long pid;
    char msg[50];
} t_mensagem;





void alarm_handler(int signum){
    printf("ocorreu timeout - nao recebi mensagem em 2 s\n");
}


void P2(int idfila){
    t_mensagem msg_env;
    int i;
    char msg_temp[50];
    int pausa;

    msg_env.pid = getpid();

    for(i=0; i<10; i++){
        //espera de 1 a 4 segundos
        pausa = rand() % 4;
        pausa++;

        // printf("pausa: %d\n", pausa);
        sleep(pausa);

        //formata mensagem
        sprintf(msg_temp, "pid=%ld numero=%d", msg_env.pid, i+1);
        strcpy(msg_env.msg, msg_temp);

        //envia mensagem
        msgsnd(idfila, &msg_env, sizeof(msg_env), 0);

        printf("mensagem %s enviada\n", msg_env.msg);
    }

    // printf("Finalizando P2\n");


}


void P1(int idfila){
    t_mensagem msg_rcv;
    int i, bytes_read;

    for(i=0; i<10; i++){
        //se nao receber em 2 segundos, TIMEOUT, envia SIGALRM - para de esperar mensagem
        alarm(2);

        //recebe mensagem
        bytes_read = msgrcv(idfila, &msg_rcv, sizeof(msg_rcv), 0, 0);

        //se leu mensagem e não deu timeout
        if(bytes_read > -1){
            //imprime conteudo da mensagem
            printf("mensagem %s recebida\n", msg_rcv.msg);
        }
    }

    // printf("Finalizando P1\n");

}


int main(){
    int pid, idfila;
    time_t t;

    //define seed para o gerador aleatorio
    srand((unsigned) time(&t));

    signal(SIGALRM, alarm_handler);


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
        P2(idfila);
    }
    else{
        //processo pai
        P1(idfila);
    }

    //processo pai P1 remove a fila
    if(pid != 0){
        msgctl(idfila, IPC_RMID, 0);
    }


    // if(pid==0){
    //     printf("teste p2 saindo\n");
    // }


    return 0;
}
