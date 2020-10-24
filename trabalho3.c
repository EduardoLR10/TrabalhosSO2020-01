/*---------------------------------------------------------------------------
  Integrantes:
    - Giordano Suffert Monteiro - 170011160;
    - Eduardo Lemos Rocha - 170009157;
    - Otho Teixeira Komatsu - 170020142
    - Pedro Lucas Pinto Andrade - 160038316;

  Versão do SO: Ubuntu?
  Versão do gcc: usei 9.3.0
---------------------------------------------------------------------------*/



void P2(){

}


void P1(){

}


int main(){
    int pid;

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
