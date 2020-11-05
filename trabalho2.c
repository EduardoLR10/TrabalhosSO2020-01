/*---------------------------------------------------------------------------
  Integrantes:
    - Giordano Suffert Monteiro - 170011160;
    - Eduardo Lemos Rocha - 170009157;
    - Otho Teixeira Komatsu - 170020142
    - Pedro Lucas Pinto Andrade - 160038316;

  Versão do SO: Ubuntu 16.04
  Versões do gcc testadas: 5.4.0 e 9.3.0
---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#define SIZE 20

struct sembuf operacao[2];
int release_read_sem_id, release_write_sem_id;

void p_sem(int sem_id)
{
  operacao[0].sem_num = 0;
  operacao[0].sem_op = 0;
  operacao[0].sem_flg = 0;

  operacao[1].sem_num = 0;
  operacao[1].sem_op = 1;
  operacao[1].sem_flg = 0;

  if (semop(sem_id, operacao, 2) < 0)
  {
    printf("Erro no P!\n");
  }
}

void v_sem(int sem_id)
{
  operacao[0].sem_num = 0;
  operacao[0].sem_op = -1;
  operacao[0].sem_flg = 0;

  if (semop(sem_id, operacao, 1) < 0)
  {
    printf("Erro no V!\n");
  }
}

int main()
{

  int pid, idshm, i;
  struct shmid_ds buf;

  int *msg;

  // Cria o semphore
  if ((release_read_sem_id = semget(0xA656, 1, IPC_CREAT | 0x1ff)) == -1)
  {
    printf("Erro na criação do semaforo de leitura!\n");
    exit(1);
  }

  if ((release_write_sem_id = semget(0xA657, 1, IPC_CREAT | 0x1ff)) == -1)
  {
    printf("Erro na criação do semaforo de escrita!\n");
    exit(1);
  }

  // Impede a leitura esperando a primeira escrita
  p_sem(release_read_sem_id);

  // Cria shared memory
  if ((idshm = shmget(0xA656, sizeof(int), IPC_CREAT | 0x1ff)) == -1)
  {
    printf("Erro na criação da memoria compartilhada!\n");
    exit(1);
  }

  if ((pid = fork()) == -1)
  {
    printf("Erro na criação do filho!\n");
    exit(1);
  }

  // FILHO
  if (pid == 0)
  {
    msg = (int *)shmat(idshm, (char *)0, 0);

    if (msg == (int *)-1)
    {
      printf("Erro no mapemanto da memoria compartilhada\n");
      exit(1);
    }

    for (i = 1; i <= SIZE; i++)
    {
      p_sem(release_write_sem_id);
      *msg = i;
      printf("processo %d escreveu %d!\n", getpid(), *msg);
      v_sem(release_read_sem_id);
    }

    exit(0);
  }

  // PAI
  msg = (int *) shmat(idshm, (char *)0, 0);

  if (msg == (int *)-1)
  {
    printf("Erro no mapemanto da memoria compartilhada\n");
    exit(1);
  }

  for (int i = 1; i <= SIZE; i++)
  {
    p_sem(release_read_sem_id);  
    printf("processo %d leu %d!\n", getpid(), *msg);
    v_sem(release_write_sem_id);
  }

  if(shmctl(idshm, IPC_RMID, NULL) < 0)
  {
    printf("Erro na exclusão da memoria compartilhada\n");
  }

  if(semctl(release_read_sem_id, 0, IPC_RMID, NULL) == -1){
    printf("Erro na exclusão do semaforo de leitura\n");
  }
  
  if (semctl(release_write_sem_id, 0, IPC_RMID, NULL) == -1)
  {
    printf("Erro na exclusão do semaforo de escrita\n");
  }
  
  return 0;
}
