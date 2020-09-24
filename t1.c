#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
  int i, pid;
  
  printf("Sou o processo pai com pid = %d\n", getpid());

  for (i=0; i<5; i++)
  {
    if ((pid = fork()) < 0) // cria o processo filho e verifica se houve erro
    {
      printf("Error na criacao do fork!");
      exit(1);
    }
    if(pid == 0) // proibe que os processos filhos tenham seus proprios filhos
    {
      printf("Sou o processo filho com pid = %d\n", getpid());
      break;
    }
  }
  
  if (sleep(30) > 0) // deixa o processo dormindo e verifica se houve chamada foi interrompida por um manipulador de sinal
  {
      printf("Sleep interrompido.");
      exit(1);
  }

}