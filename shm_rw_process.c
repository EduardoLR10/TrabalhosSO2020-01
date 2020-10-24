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
  union semun
  {
    int val;
    struct semid_ds *buf;
    ushort *array;
  } arg;
  int *msg;

  // SEMAPHORE Creation
  if ((release_read_sem_id = semget(0xA656, 1, IPC_CREAT | 0x1ff)) < -1)
  {
    printf("Erro na criação do semaforo!\n");
    exit(1);
  }

  if ((release_write_sem_id = semget(0xA657, 1, IPC_CREAT | 0x1ff)) < -1)
  {
    printf("Erro na criação do semaforo!\n");
    exit(1);
  }

  // Forbids first read until first write
  p_sem(release_read_sem_id);

  // SHARED MEMORY Creation
  if ((idshm = shmget(0xA656, sizeof(int), IPC_CREAT | 0x1ff)) < 0)
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


  msg = (int *) shmat(idshm, (char *)0, 0);

  // PAI
  for (int i = 1; i <= SIZE; i++)
  {
    p_sem(release_read_sem_id);  
    printf("processo %d leu %d!\n", getpid(), *msg);
    v_sem(release_write_sem_id);
  }

  if (msg == (int *)-1)
  {
    printf("Erro no mapemanto da memoria compartilhada\n");
  }

  if (semctl(release_write_sem_id, 0, IPC_RMID, arg) < 0)
  {
    printf("Erro na exclusão do semaforo\n");
  }

  if(semctl(release_read_sem_id, 0, IPC_RMID, arg)){
    printf("Erro na exclusão do semaforo\n");
  }
  

  return 0;
}