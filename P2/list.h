#include <time.h>

#ifndef MAXARRAY
#define MAXARRAY 4096
#define MAXLINE 2048
#define MAXNAME 1024
#endif

//Array implementation and commands
struct POINTERARRAY{
  char *array_of_pointers[MAXARRAY];
	int currentLength;
} arr;

struct MEMORY{
  void * pointer;
  size_t size;
  char * name;
  char * op;
  //char * filename;
  int key;
  struct tm time;
  char filename[];
};

struct MEMORYARRAY{
  struct MEMORY *array_of_pointers[MAXARRAY];
	int currentLength;
} arrmem;

void CreateArray();
void InsertInArray(char x[],long int n);
void ClearArray();

void CreateMemoryArray();
int isEmptyMemArray();
int isFullMemArray();
void InsertInMemoryArray(struct MEMORY * pointer);
void ClearMemoryArray();
char* DayOfTheWeek(int i);
char* MonthOfTheYear(int i);
void deleteAtPosition(int j);
void PrintMmap(int i);
void PrintMalloc(int i);
void PrintShared(int i);
void PrintAll();
void PrintMemoryArray(char * op);

