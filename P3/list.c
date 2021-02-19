#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>
#include <pwd.h> /*get owner name*/
#include <grp.h> /*get group name*/
#include <libgen.h> /*get filename*/
#include <ctype.h>/*isdigit*/
#include <sys/mman.h>/*PROT_READ*/
#include <fcntl.h>/*O_RDONLY*/
#include <sys/ipc.h>/*shmctl*/
#include <sys/shm.h>/*shmctl*/
#include <sys/wait.h> /*waitpid*/

#define MAXARRAY 4096

struct POINTERARRAY{
  char *array_of_pointers[MAXARRAY];
	int currentLength;
} arr;

struct MEMORY{
  void * pointer;
  size_t size;
  char * name;
  char * op;
  int key;
  struct tm time;
  char filename[32];
};

struct MEMORYARRAY{
  struct MEMORY *array_of_pointers[MAXARRAY];
	int currentLength;
} arrmem;

struct PROC{
  pid_t pid;
  int priority;
  char * name;
  struct tm time;
};

struct node{
    struct PROC * elem;
    struct  node * next ;
};
typedef struct node *position;
typedef struct node *list;

static struct node * CreateNode(){
    struct node * tmp = malloc(sizeof(struct node));
    if (tmp==NULL){
        printf("Out of space\n");
        exit(EXIT_FAILURE);
    }
    return tmp;
}

list CreateList(){
    struct node * l =CreateNode();
    l->next=NULL;
    return l;
}

int IsEmptyList(list l){
    return (l->next==NULL);
}

position First (list l){
    return l->next;
}

position Last (list l){
  if(IsEmptyList(l))
    return l;
  else{
    struct node *tmp=CreateNode();
    tmp=First(l);
    while(tmp->next!=NULL){
      tmp=tmp->next;
    }
    return tmp;
  }
}


char* DayOfTheWeek(int i){
    switch (i){
      case 0:return "Sun";
      case 1:return "Mon";
      case 2:return "Tue";
      case 3:return "Wed";
      case 4:return "Thu";
      case 5:return "Fri";
      case 6:return "Sat";
    }
    return NULL;
}

char* MonthOfTheYear(int i){
    switch (i){
      case 0:return "Jan";
      case 1:return "Feb";
      case 2:return "Mar";
      case 3:return "Apr";
      case 4:return "May";
      case 5:return "Jun";
      case 6:return "Jul";
      case 7:return "Agu";
      case 8:return "Sep";
      case 9:return "Oct";
      case 10:return "Nov";
      case 11:return "Dec";
    }
    return NULL;
}

void InsertInList (struct PROC * x, list l){
    struct node * tmp=CreateNode();
    tmp->elem=x;
    tmp->next=NULL;
    Last(l)->next=tmp;
}

position Find(list l, pid_t pid){
    struct node *p = l->next;
    while(p!=NULL && p->elem->pid!=pid)
        p=p->next;
    return p;
}

/*static position FindPrevious(list l, void * x,int(*comp)(const void *, const void *)){
    struct node * p=l;
    while(p->next!=NULL && 0!=(*comp)(p->next->elem,x))
        p=p->next;
    return p;
}*/

static int IsLast(struct node * p){
    return(p->next==NULL);
}

/*void Delete(list l, void * x,int(*comp)(const void *,const void *)){
    struct node * tmp, * p =FindPrevious(l,x,comp);
    if (!IsLast(p)){
        tmp=p->next;
        p->next=tmp->next;
        free(tmp);
    }
}*/

position Next(position p){
    return p->next;
}

int IsEndOfList(position p){
    return (p==NULL);
}

void PrintListProcs(list l){
    if(!IsEmptyList(l)){
      struct node *tmp=CreateNode();
      tmp=First(l);
      while(tmp!=NULL){
        printf("%d p=%d %s %s %d %02d:%02d:%02d %d %s\n",
            tmp->elem->pid,tmp->elem->priority,DayOfTheWeek(tmp->elem->time.tm_wday),
            MonthOfTheYear(tmp->elem->time.tm_mon),tmp->elem->time.tm_mday,
            tmp->elem->time.tm_hour,tmp->elem->time.tm_min,tmp->elem->time.tm_sec,
            tmp->elem->time.tm_year+1900,tmp->elem->name);
        tmp=tmp->next;
      }
    }
}

void PrintProc(list l,pid_t pid){
    if(!IsEmptyList(l)){
      struct node *tmp=CreateNode();
      tmp=Find(l,pid);
      printf("%d p=%d %s %s %d %02d:%02d:%02d %d %s\n",
          tmp->elem->pid,tmp->elem->priority,DayOfTheWeek(tmp->elem->time.tm_wday),
          MonthOfTheYear(tmp->elem->time.tm_mon),tmp->elem->time.tm_mday,
          tmp->elem->time.tm_hour,tmp->elem->time.tm_min,tmp->elem->time.tm_sec,
          tmp->elem->time.tm_year+1900,tmp->elem->name);
    }
}



void CreateArray(){
  arr.currentLength = 0;
}

void CreateMemoryArray(){
  arrmem.currentLength = 0;
}

int isEmptyMemArray(){
  if(arrmem.currentLength==0)
    return 1;
  else
    return 0;
}

int isFullMemArray(){
  if(arrmem.currentLength==(MAXARRAY))
    return 1;
  else
    return 0;
}

void InsertInArray(char x[],long int n){
  arr.array_of_pointers[arr.currentLength]=(char *)malloc(n*sizeof(char));
  strcpy(arr.array_of_pointers[arr.currentLength],x);
  arr.currentLength++;
}

void ClearArray(){
  for(int i = 0; i<arr.currentLength;i++){
    free(arr.array_of_pointers[i]);
  }
  arr.currentLength=0;
}

void InsertInMemoryArray(struct MEMORY * pointer){
  if(!isFullMemArray()){
    arrmem.array_of_pointers[arrmem.currentLength]=pointer;
    arrmem.currentLength++;
  }
}

void ClearMemoryArray(){
  for(int i = 0; i<arrmem.currentLength;i++){
    free(arrmem.array_of_pointers[i]->pointer);
    free(arrmem.array_of_pointers[i]);
  }
  arrmem.currentLength=0;
}

void deleteAtPosition(int j){
  for(int i = j; i < arrmem.currentLength; i++){
    arrmem.array_of_pointers[i]=arrmem.array_of_pointers[i+1];
  }
  arrmem.currentLength--;
}

void PrintMmap(int i){
  printf("%p: size:%ld. %s filename:%s fd:%d %s %s %d %02d:%02d:%02d %d\n", arrmem.array_of_pointers[i]->pointer,
  arrmem.array_of_pointers[i]->size,arrmem.array_of_pointers[i]->op,arrmem.array_of_pointers[i]->filename,
  arrmem.array_of_pointers[i]->key,
  DayOfTheWeek(arrmem.array_of_pointers[i]->time.tm_wday),MonthOfTheYear(arrmem.array_of_pointers[i]->time.tm_mon),
  arrmem.array_of_pointers[i]->time.tm_mday,arrmem.array_of_pointers[i]->time.tm_hour,
  arrmem.array_of_pointers[i]->time.tm_min,arrmem.array_of_pointers[i]->time.tm_sec,
  arrmem.array_of_pointers[i]->time.tm_year+1900);
}

void PrintMalloc(int i){
  printf("%p: size:%ld. %s %s %s %d %02d:%02d:%02d %d\n", arrmem.array_of_pointers[i]->pointer,
  arrmem.array_of_pointers[i]->size,arrmem.array_of_pointers[i]->op,
  DayOfTheWeek(arrmem.array_of_pointers[i]->time.tm_wday),MonthOfTheYear(arrmem.array_of_pointers[i]->time.tm_mon),
  arrmem.array_of_pointers[i]->time.tm_mday,arrmem.array_of_pointers[i]->time.tm_hour,
  arrmem.array_of_pointers[i]->time.tm_min,arrmem.array_of_pointers[i]->time.tm_sec,
  arrmem.array_of_pointers[i]->time.tm_year+1900);
}

void PrintShared(int i){
  printf("%p: size:%ld. %s key:%d %s %s %d %02d:%02d:%02d %d\n", arrmem.array_of_pointers[i]->pointer,
  arrmem.array_of_pointers[i]->size,arrmem.array_of_pointers[i]->op,
  arrmem.array_of_pointers[i]->key,
  DayOfTheWeek(arrmem.array_of_pointers[i]->time.tm_wday),MonthOfTheYear(arrmem.array_of_pointers[i]->time.tm_mon),
  arrmem.array_of_pointers[i]->time.tm_mday,arrmem.array_of_pointers[i]->time.tm_hour,
  arrmem.array_of_pointers[i]->time.tm_min,arrmem.array_of_pointers[i]->time.tm_sec,
  arrmem.array_of_pointers[i]->time.tm_year+1900);
}

void PrintAll(){
  char * op;
  for(int i = 0; i < arrmem.currentLength; i++){
    op=arrmem.array_of_pointers[i]->op;
    if(strcmp("mmap",op)==0)
      PrintMmap(i);
    else if(strcmp("malloc",op)==0)
      PrintMalloc(i);
    else if(strcmp("shared",op)==0)
      PrintShared(i);
  }
}

void PrintMemoryArray(char * op){
  if(op[0]=='-') op++;/*removes the first - from the opcode if it exists*/
  for(int i = 0; i < arrmem.currentLength; i++){
    if(strcmp("asignar",op)!=0){
      if(strcmp(arrmem.array_of_pointers[i]->op,op)!=0) continue;
    }else
      break;
    if(strcmp("mmap",op)==0)
      PrintMmap(i);
    else if(strcmp("malloc",op)==0)
      PrintMalloc(i);
    else if(strcmp("shared",op)==0){
      PrintShared(i);
    }
  }
  if(strcmp("asignar",op)==0)
    PrintAll();
}
