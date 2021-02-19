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
