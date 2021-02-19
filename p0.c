//Autores: Antón Concheiro Fernández anton.concheiro
//         Martín Cabanas Fernández martin.cfernandez

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define MAXARRAY 4096
#define MAXLINE 2048
#define MAXNAME 1024

struct COMMAND{
    char *name;
    void (*pfunc) (char *tr[]);
};

//Array implementation and commands
/*struct POINTERARRAY{
	COMMAND *list[MAXARRAY];
	int currentLength;
};

int CreateArray(){
  int *array_of_pointers[MAXARRAY];
}

int InsertInArray(){
  array_of_pointers[]=&a;
  a=cmd[i];
}

int RemoveInArray(){

}

int PrintArray(){
    for(int i = 0; i<MAXARRAY;i++){
      printf("%d\n",*array_of_pointers[i]);
    }
}

int ClearArray(){

}
*/
//Commands
void CmdAutores(char *tr[]){
    if(tr[0] != NULL && strcmp(tr[0],"-n")==0){
        printf("Antón Concheiro\nMartín Cabanas\n");
    }else if(tr[0] != NULL && strcmp(tr[0],"-l")==0){
        printf("login1: anton.concheiro\nlogin2: martin.cfernandez\n");
    }else{
        printf("Antón Concheiro\nlogin1: anton.concheiro\nMartín Cabanas\nlogin2: martin.cfernandez\n");
    }
}

void CmdPid(char *tr[]){
    if(tr[0] != NULL && strcmp(tr[0],"-p")==0)
        printf("parent process pid: %d\n",getppid());
    else
        printf("process pid: %d\n",getpid());
}

void CmdDir (char *tr[]){
    char dir[MAXNAME];
    if (tr[0]==NULL)
        printf("%s\n",getcwd(dir,MAXNAME));
    else
    if (chdir(tr[0])==-1)
        perror("Cannot change directiory");
}

void CmdFecha(char *tr[]){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("fecha: %d-%d-%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);

}

void CmdHora(char *tr[]){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("hora: %d:%d:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void CmdHist(char *tr[]){
    PrintArray();
}

void CmdFin(char *tr[]){
    exit(0);
}

void CmdEnd(char *tr[]){
    exit(0);
}

void CmdExit(char *tr[]){
    exit(0);
}

int TrocearCadena(char * cadena, char * tr[]){
    int i=1;
    if ((tr[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((tr[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

struct COMMAND cmd[]={
    {"autores",CmdAutores},
    {"pid",CmdPid},
    {"cdir",CmdDir},
    {"fecha",CmdFecha},
    {"hora",CmdHora},
    {"hist",CmdHora},
    {"fin",CmdFin},
    {"end",CmdEnd},
    {"exit",CmdExit},
    {NULL,NULL}
};

void ProcessInput(char *inp){
    char * tr[MAXLINE/2];

    if (TrocearCadena(inp,tr)==0)
        return;
    for(int i=0;cmd[i].name!=NULL;i++)
        if(strcmp(tr[0],cmd[i].name)==0){
			       //InsertInArray(cmd[i]);
            (*cmd[i].pfunc)(tr+1);
            return;
        }

    printf("%s not found\n",tr[0]);
}

int main(int argc, char *argv[]){
    char input [MAXLINE];
    int i;

    for(i=0;i<argc;i++)
        printf("main's %dth argument value is:%s\n",i,argv[i]);

    while(1){
        printf("#) ");
        fgets(input,MAXLINE,stdin);
        ProcessInput(input);
    }
}
