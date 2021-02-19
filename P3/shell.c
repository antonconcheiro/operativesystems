//Autores: AntÃ³n Concheiro FernÃ¡ndez anton.concheiro 53776002R
//         MartÃ­n Cabanas FernÃ¡ndez martin.cfernandez 49204428E

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
#include <sys/ipc.h>/*schmget*/
#include <sys/shm.h>/*schmget*/
#include <errno.h>
#include <sys/wait.h> /*waitpid*/
#include <sys/resource.h> /*setpriority*/
#include "list.h"

#define LEERCOMPLETO ((ssize_t)-1)

struct COMMAND{
    char *name;
    void (*pfunc) ();
};


//global Variables
int ag,bg,cg;

//Commands
void CmdAutores(char *tr[]){
    if(tr[0] != NULL && strcmp(tr[0],"-n")==0){
        printf("AntÃ³n Concheiro\nMartÃ­n Cabanas\n");
    }else if(tr[0] != NULL && strcmp(tr[0],"-l")==0){
        printf("login1: anton.concheiro\nlogin2: martin.cfernandez\n");
    }else{
        printf("AntÃ³n Concheiro\nlogin1: anton.concheiro\nMartÃ­n Cabanas\nlogin2: martin.cfernandez\n");
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
    /*set variable day of type time_t to NULL*/
    time_t day = time(NULL);
    /*localtime returns the current time to date, with structure tm*/
    struct tm date = *localtime(&day);
    /*months are from 0-11 and year starts counting in 1900*/
    printf("fecha: %d/%d/%d\n", date.tm_mday, date.tm_mon + 1,date.tm_year + 1900);

}

void CmdHora(char *tr[]){
    /*set variable sec of type time_t to the current time*/
    time_t sec = time(&sec);
    /*localtime returns the current time to hour, with structure tm*/
    struct tm hour = *localtime(&sec);
    printf("hora: %02d:%02d:%02d\n", hour.tm_hour, hour.tm_min, hour.tm_sec);
}

void CmdHist(char *tr[]){
  if(tr[0] != NULL && strcmp(tr[0],"-c")==0){
    ClearArray();
  }else{
    for(int i = 0; i < arr.currentLength; i++){
        printf("%s\n", arr.array_of_pointers[i]);
    }
  }
}

void CmdFin(){
    ClearArray();
    ClearMemoryArray();
    exit(0);
}

//CAMBIADA DE SITIO
int IsDirectory(char *dir){
    struct stat path_stat;
    if(lstat(dir, &path_stat)==-1)
		return 0;
    return S_ISDIR(path_stat.st_mode);
}

void ListDir(char *path){
  struct dirent *dir;//Struct used to access the name of files and directories
  DIR *d;
  char *newDir;
  if(path != NULL){
	  newDir = path;
  } else {
	  newDir = (".");
  }
  d=opendir(newDir);//Opens the directory specified. With . it opens current dir
  if(d!=NULL){//If directory can be accessed
    printf("********* .\n");
    while((dir=readdir(d))!=NULL)//Loops until there is not anything to list
      printf("%s \n",dir -> d_name/*,buf.st_size*/);//Prints each file or directory
    if(closedir(d)!=0){//Closes the opendir, if -1 there is an error
        perror("Error closing directory");
    }
  }
  else{
    perror("Cannot open directory");
  }
}

void CmdCrear(char *tr[]){
  if ((tr[0] != NULL) && (strcmp(tr[0],"-d")==0)){//Check for -d
    if(mkdir(tr[1],0755)!=0) /*Creates a dir with the given name and 0755 permissions and checks if it returns -1 in case of an error*/
      perror("Cannot create directory");
    }
  else if (tr[0]!=NULL){//Check for a name
    FILE *fp;//Creates a pointer fp of type file
    if((fp = fopen(tr[0],"w+"))==NULL)/*Opens a file assigned to fp with the given name and w+ permission and checks if NULL in case of an error*/
      perror("Cannot create file");
  }else
    ListDir(NULL);
}

void Rm_Dir(char *path){
	int len = 0;
	DIR *d; d=opendir(path);
	if (d!=NULL){/*if directory could be opened*/
		struct dirent *p;/*creates a dirent struct to access each file name*/
		while ((p=readdir(d))!=NULL){
      if(strncmp(p->d_name, ".",1)==0) continue;
			len = strlen(path) + strlen(p->d_name) + 2; /*sets len to length of path + name the file + 2, to store "/" and null char*/
			char *buf;/*buffer string to store each file and delete it*/
			buf = malloc(len);
			if (buf!=NULL){/*checks if memory could be allocated*/
				snprintf(buf,len,"%s/%s",path,p->d_name);
				struct stat statbuf;/*creates stat struct to check if file is a directory*/
				if (lstat(buf,&statbuf)==0){/*if 0 it means it was executed correctly, meaning buf value is stored in statbuf*/
          if (S_ISDIR(statbuf.st_mode)) Rm_Dir(buf);
					else if(remove(buf)!=0) perror("Could not delete file");
				}
				free(buf);/*free the pointer buf in memory*/
			}else
			   perror("Lack of memory");/*Error in case of not enough memory*/
		}
		if(closedir(d)!=0)/*closes the opened dir, in case of error prints it*/
			perror("Could not close directory");
	}
	if(remove(path)!=0 && rmdir(path)!=0)
		perror("Could not delete directory");
}

void CmdBorrar(char *tr[]){
  /*delete with -r*/
  if(tr[0] != NULL && strcmp(tr[0],"-r")==0){
    Rm_Dir(tr[1]);
   /*delete without -r*/
 }else if(tr[0] != NULL){
    if(remove(tr[0])!=0)/*deletes file/dir and checks return in case of error*/
      perror("Could not delete file");
   /*delete without parameters*/
  }else{
    ListDir(NULL);/*Lists the contents in current directory*/
  }
}

char TipoFichero (mode_t m){
  switch (m&S_IFMT) {
   /*and bit a bit con los bits de formato,0170000 */
   case S_IFSOCK:  return 's';/*socket */
   case S_IFLNK:   return 'l';/*symbolic link*/
   case S_IFREG:   return '-';/* fichero normal*/
   case S_IFBLK:   return 'b';/*block device*/
   case S_IFDIR:   return 'd';/*directorio */
   case S_IFCHR:   return 'c';/*char  device*/
   case S_IFIFO:   return 'p';/*pipe*/
   default: return '?';/*desconocido, no deberia aparecer*/
 }
}

//CREADA, NECESARIA PARA doINFO
char* FilePermissions(struct stat buf){
	static char str[12];
    strcpy (str,"---------- ");
    str[0]=TipoFichero(buf.st_mode);
    /*calls TipoFichero to know what kind of file it is*/
    if (buf.st_mode & S_IRUSR ) str[1] = 'r';
    if (buf.st_mode & S_IWUSR ) str[2] = 'w';
    if (buf.st_mode & S_IXUSR ) str[3] = 'x';
    if (buf.st_mode & S_IRGRP ) str[4] = 'r';
    if (buf.st_mode & S_IWGRP ) str[5] = 'w';
    if (buf.st_mode & S_IXGRP ) str[6] = 'x';
    if (buf.st_mode & S_IROTH ) str[7] = 'r';
    if (buf.st_mode & S_IWOTH ) str[8] = 'w';
    if (buf.st_mode & S_IXOTH ) str[9] = 'x';
	return str;
}

void doInfoLongList(struct stat buf,char str[12],char *tr,char* filename){
  struct tm time;
  struct stat stats_link;
  printf("%ld ",buf.st_ino);/*prints inode number*/
  for(int a=0;a<12;a++)
    printf("%c",str[a]);/*prints the string of file permissions*/
  struct passwd *pwd;/*struct of type passwd to get the username*/
  pwd = getpwuid(buf.st_uid);
  struct group *grp;/*struct of type group to get the group name*/
  grp = getgrgid(buf.st_gid);
  if(pwd == NULL || grp == NULL)
     return;
  time=*(gmtime(&buf.st_mtime));
  /*print the rest of the info*/
  printf(" %ld %s %s %ld %d/%d/%d %d:%d:%d",buf.st_nlink,pwd->pw_name,grp->gr_name,buf.st_size,time.tm_mday,time.tm_mon +1 ,time.tm_year+1900,time.tm_hour,time.tm_min,time.tm_sec);
  if(TipoFichero(buf.st_mode)=='l'){
    stat(tr,&stats_link);
    char *linkName = malloc(stats_link.st_size +1);
    stats_link.st_size=readlink(tr,linkName,stats_link.st_size +1);
    printf(" %s -> %s\n",filename,linkName);
  }else
    printf(" %s\n",filename);
}

void doInfo(char *tr, int longlist){
  FILE *file;/*struct FILE to get the details of each file*/
  if ((file = fopen(tr, "r"))){/*tries to open the file at pos i*/
    struct stat buf;
    stat(tr, &buf);
    char * str = FilePermissions(buf);
    /*get the file name*/
    char* local_file = tr;
    char* ts = strdup(local_file);
    char* filename = basename(ts);
    if(longlist){
      doInfoLongList(buf,str,tr,filename);
    }else{
      int size = buf.st_size;
      printf("%s %d\n",filename,size);
    }
    fclose(file);
  }else{
    perror("Could not open file");
  }
}

void CmdInfo(char *tr[]){
  if(tr[0] != NULL){/*check if there is a name specified*/
    int x;
    for (x = 0; tr[x] != NULL; x++)
      doInfo(tr[x],0);
  }
}

void FilterList(char *path, int infoList, int rec, int avoidHid){
	int len = 0;
	DIR *d; struct dirent *p;
	if((d=opendir(path))==NULL){
		perror("Cannot open directory");
		return;
	}
	while((p=readdir(d))!=NULL){
		if(avoidHid==1 && strncmp(p->d_name, ".",1)==0)
			continue;
		len = strlen(path) + strlen(p->d_name) + 2;
		char *buf;/*buffer string to store each file*/
		buf = malloc(len);
		snprintf(buf,len,"%s/%s",path,p->d_name);
		doInfo(buf,infoList);
		if(rec && IsDirectory(buf) && strncmp(p->d_name, ".",1)!=0 ){
			printf("%s\n",p->d_name);
			printf("********* %s\n",p->d_name);
			FilterList(buf, infoList, rec, avoidHid);
		}
		free(buf);
	}
	if(closedir(d)!=0)//Closes the opendir, if -1 there is an error
		perror("Error closing directory");
}

void CmdListar(char *tr[]){
  int x = 0; int infoList = 0; int rec = 0; int avoidHid = 0;
  while(tr[x]!=NULL){
		if(strcmp(tr[x], "-l") == 0) infoList = 1;
    else if(strcmp(tr[x], "-r") == 0) rec= 1;
    else if(strcmp(tr[x], "-v") == 0) avoidHid = 1;
    else break;
		x++;
  }
  if(tr[x] != NULL){
   while (tr[x] != NULL){
     if (IsDirectory(tr[x])){
        printf("********* %s\n",tr[x]);
        FilterList(tr[x], infoList, rec, avoidHid);
     }else
		     doInfo (tr[x], infoList);
     x++;
  }
 }else{
   printf("********* .\n");
   FilterList(".", infoList, rec, avoidHid);
  }
}

void insertMmap(void * p,size_t space,char fichero[],int df){
  struct MEMORY * mempos=malloc(sizeof(struct MEMORY));
  mempos->pointer=p; mempos->size=space;mempos->op="mmap";
  strcpy(mempos->filename,fichero);mempos->key=df;
  time_t sec = time(&sec); struct tm hour = *localtime(&sec);
  mempos->time=hour;
  InsertInMemoryArray(mempos);
}

void * MmapFichero (char * fichero, int protection){
  int df, map=MAP_PRIVATE,modo=O_RDONLY;
  struct stat s;
  void *p;
  if (protection&PROT_WRITE)
    modo=O_RDWR;
  if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
    return NULL;
  if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
    return NULL;
  insertMmap(p,s.st_size,fichero,df);
  return p;
}

void Cmd_AsignarMmap (char *arg[]){
  char *perm;
  void *p;
  int protection=0;
  if (arg[1]==NULL){
    PrintMemoryArray(arg[0]);
    return;
  }
  if ((perm=arg[1])!=NULL && strlen(perm)<4) {
    if (strchr(perm,'r')!=NULL)
     protection|=PROT_READ;
    if (strchr(perm,'w')!=NULL)
     protection|=PROT_WRITE;
    if (strchr(perm,'x')!=NULL)
     protection|=PROT_EXEC;
  }
  if ((p=MmapFichero(arg[1],protection))==NULL)
    perror ("Imposible mapear fichero");
  else
    printf ("fichero %s mapeado en %p\n", arg[1], p);
}

void insertMalloc(void * p,size_t space){
  struct MEMORY * mempos=malloc(sizeof(struct MEMORY));
  mempos->pointer=p; mempos->size=space; mempos->op="malloc";
  mempos->name=""; mempos->key=0; time_t sec = time(&sec);
  struct tm hour = *localtime(&sec); mempos->time=hour;
  InsertInMemoryArray(mempos);
}

void AsignarMalloc(char *tr[]){
  if(tr[1]!=NULL && isdigit(*tr[1])){/*asignar -malloc [size]*/
    size_t space = atoi(tr[1]);/*convert tr[1] to int*/
    void* p = malloc(atoi(tr[1]));
    if(p==NULL)
      perror("Memory not allocated.\n");
    else{
      insertMalloc(p,space);
      printf("allocated %ld at %p\n",space,p);
    }
  }else{/*asignar -malloc*/
    PrintMemoryArray(*tr);
  }
}

void insertShared(void *p,size_t tam, key_t clave){
  struct MEMORY * mempos=malloc(sizeof(struct MEMORY));
  mempos->pointer=p; mempos->op="shared"; mempos->key=clave;
  time_t sec = time(&sec); struct tm hour = *localtime(&sec);
  mempos->time=hour; mempos->size=tam;
  InsertInMemoryArray(mempos);
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam){
  void * p;
  int aux,id,flags=0777;
  struct shmid_ds s;
  if (tam)
    flags=flags | IPC_CREAT | IPC_EXCL;
  if (clave==IPC_PRIVATE){/*no nos vale*/
    errno=EINVAL;
    return NULL;
  }
  if ((id=shmget(clave, tam, flags))==-1)
    return (NULL);
  if ((p=shmat(id,NULL,0))==(void*) -1){
    aux=errno;
    if (tam) shmctl(id,IPC_RMID,NULL);/*se borra */
    errno=aux;
    return (NULL);
  }
  shmctl (id,IPC_STAT,&s);
  insertShared(p,s.shm_segsz,clave);
  return (p);
}

void Cmd_AsignarCreateShared (char *arg[]){
  key_t k;
  size_t tam=0;
  void *p;
  if ((!strcmp(arg[0],"-crearshared") && (arg[1]==NULL || arg[2]==NULL) )||
      (!strcmp(arg[0],"-shared") && arg[1]==NULL)){
    PrintMemoryArray("shared");
    return;
  }
  k=(key_t) atoi(arg[1]);
  if (!strcmp(arg[0],"-crearshared") && arg[1]!=NULL && arg[2]!=NULL)
    tam=(size_t) atoll(arg[2]);
  if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
    perror ("Imposible obtener memoria shmget");
  else
    printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}

void CmdAsignar(char *tr[]){
  if(tr[0] != NULL && strcmp(tr[0],"-malloc")==0)/*asignar -malloc ...*/
    AsignarMalloc(tr);
  else if(tr[0] != NULL && strcmp(tr[0],"-mmap")==0)/*asignar -mmap ...*/
    Cmd_AsignarMmap(tr);
  else if((tr[0] != NULL && (!strcmp(tr[0],"-crearshared"))) ||
         (tr[0] != NULL &&  !strcmp(tr[0],"-shared")))/*asignar -crearshared ...*/
   Cmd_AsignarCreateShared(tr);
  else
    PrintMemoryArray("asignar");
}

int deleteMalloc(size_t space){
  for(int i = 0; i < arrmem.currentLength; i++){
    if(strcmp(arrmem.array_of_pointers[i]->op,"malloc")!=0) continue;
    if(arrmem.array_of_pointers[i]->size == space){
      printf("block at address %p deallocated (malloc)\n",arrmem.array_of_pointers[i]->pointer);
      free(arrmem.array_of_pointers[i]->pointer);
      deleteAtPosition(i);
      return 0;
    }
  }
  return -1;
}

void desasignarMalloc(char *tr[]){
  if(tr[1]!=NULL && isdigit(*tr[1])){
    size_t space = atoi(tr[1]);/*convert tr[1] to int*/
    if(deleteMalloc(space)!=0)
      perror("Could not delete memory space");
  }else
    PrintMemoryArray(*tr);
}

int deleteMmap(char * file){
  for(int i = 0; i < arrmem.currentLength; i++){
    if(strcmp(arrmem.array_of_pointers[i]->op,"mmap")!=0) continue;
    if(strcmp(arrmem.array_of_pointers[i]->filename,file)==0){
      if(munmap(arrmem.array_of_pointers[i]->pointer,arrmem.array_of_pointers[i]->size)!=0){
        perror("Could not delete memory");
        return -1;
      }
      printf("block at address %p deallocated (mmap)\n",arrmem.array_of_pointers[i]->pointer);
      deleteAtPosition(i);
      return 0;
    }
  }
  return -1;
}

void desasignarMmap(char *tr[]){
  if(tr[1]!=NULL){
    if(deleteMmap(tr[1])!=0)
      printf("File %s not mapped\n",tr[1]);
  }else
    PrintMemoryArray(*tr);
}

int deleteSharedArray(int key){
  for(int i = 0; i < arrmem.currentLength; i++){
    if(strcmp(arrmem.array_of_pointers[i]->op,"shared")!=0) continue;
    if(arrmem.array_of_pointers[i]->key==key){
        printf("block at address %p deallocated (shared)\n",arrmem.array_of_pointers[i]->pointer);
        deleteAtPosition(i);
        i--;
    }
  }
  return 0;
}

int borrarShared(int key){
  for(int i = 0; i < arrmem.currentLength; i++){
    if(strcmp(arrmem.array_of_pointers[i]->op,"shared")!=0) continue;
    if(arrmem.array_of_pointers[i]->key==key){
      if(shmdt(arrmem.array_of_pointers[i]->pointer)!=0)
        return -1;
      else{
        printf("block at address %p deallocated (shared)\n",arrmem.array_of_pointers[i]->pointer);
        deleteAtPosition(i);
        return 0;
      }
    }
  }
  return -1;
}

void desasignarShared(char *tr[]){
  if(tr[1]!=NULL){
    if(borrarShared(atoi(tr[1]))!=0)
      perror("Could not delete shared memory");
  }else
    PrintMemoryArray(*tr);
}

int deleteAddr(void *p){
  for(int i = 0; i < arrmem.currentLength; i++){
    if(arrmem.array_of_pointers[i]->pointer==p){
      if(strcmp(arrmem.array_of_pointers[i]->op,"malloc")==0){
        deleteMalloc(arrmem.array_of_pointers[i]->size);
        return 0;
      }else if(strcmp(arrmem.array_of_pointers[i]->op,"mmap")==0){
        deleteMmap(arrmem.array_of_pointers[i]->filename);
        return 0;
      }else if(strcmp(arrmem.array_of_pointers[i]->op,"shared")==0){
        borrarShared(arrmem.array_of_pointers[i]->key);
        return 0;
      }
    }
  }
  return -1;
}

void desasignarAddr(char *tr[]){
  if(deleteAddr((char*)strtoul(tr[0],NULL,16))!=0)
    perror("Could not delete memory");
}

void Cmd_borrakey (char *args[]){
  key_t clave;
  int id;
  char *key=args[0];
  if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
    printf ("   rmkey  clave_valida\n");
    return;
  }
  if ((id=shmget(clave,0,0666))==-1){
    perror ("shmget: imposible obtener memoria compartida");
    return;
  }
  if (shmctl(id,IPC_RMID,NULL)==-1 || deleteSharedArray(atoi(args[0]))!=0)
    perror ("shmctl: imposible eliminar memoria compartida\n");
  else
    printf("key %d deleted\n",atoi(args[0]));
}

void PrintMem(){
  int a,b,c;
  printf("Local variables  \t%p,\t%p,\t%p\n",&a,&b,&c);
  printf("Global variables\t%p,\t%p,\t%p\n",&ag,&bg,&cg);
  printf("Program functions\t%p,\t%p,\t%p\n",CmdAutores,CmdPid,CmdDir);
  printf("Library functions\t%p,\t%p,\t%p\n",printf,perror,atoi);
}

void CmdMem(char *tr[]){
  if(tr[0]!=NULL){
    int x = 0;
    while(tr[x]!=NULL){
        if(strcmp(tr[x], "-malloc") == 0){
          x++;
          PrintMemoryArray("malloc");
        }else if(strcmp(tr[x], "-mmap") == 0){
          x++;
          PrintMemoryArray("mmap");
        }else if(strcmp(tr[x], "-shared") == 0){
          x++;
          PrintMemoryArray("shared");
        }else if(strcmp(tr[x], "-all") == 0){
          x++;
          PrintMemoryArray("asignar");
        }else
          break;
    }
  }else
    PrintMem();
}

void CmdDesasignar(char *tr[]){
  if(tr[0] != NULL && strcmp(tr[0],"-malloc")==0)
    desasignarMalloc(tr);
  else if(tr[0] != NULL && strcmp(tr[0],"-mmap")==0)
    desasignarMmap(tr);
  else if(tr[0] != NULL && strcmp(tr[0],"-shared")==0)
    desasignarShared(tr);
  else if(tr[0] != NULL){
    desasignarAddr(tr);
  }
  else{
    PrintMemoryArray("asignar");
 }
}

void showContent(int i, int j,char* mem){
  i=i*25;
  for(int k=i;k<(i+j);k++){
    if(isprint(mem[k]))
      printf("%3c",mem[k]);
    else
      printf("  ");
  }
}

void showHexContent(int i, int j,char* mem){
  i=i*25;
  for(int k=i;k<(i+j);k++){
    printf("%3x",mem[k]);
  }
}

int calculateLines(int showLen){
  int linesShown;
  linesShown=showLen/25;
  if((showLen % 25)!=0)
    linesShown++;
  return linesShown;
}

void CmdVolcar(char *tr[]){
  char* mem=(char*)strtoul(tr[0],NULL,16);
  int showLen = 25;
  int linesShown=0;
  int charShown=0;

	if (tr[1]!=NULL)
		showLen = atoi(tr[1]);

  linesShown=calculateLines(showLen);

  for(int j= 0;j<linesShown;j++){
    if(j!=linesShown-1){
      showContent(j,25,mem);
      printf("\n");
      showHexContent(j,25,mem);
      charShown+=25;
    }else{
      showContent(j,showLen-charShown,mem);
      printf("\n");
      showHexContent(j,showLen-charShown,mem);
    }
    printf("\n");
  }
}

void CmdLlenar(char *tr[]){
	int byteN = 65;
	size_t filLen = 128;
  void * p;
  char input[3]="";

	if(tr[0]!=NULL && tr[1]!=NULL && tr[2]!=NULL){
    if(strncmp(tr[2],"'",1)==0){
      strncpy(input,tr[2],3);
      byteN=(int)input[1];
    }else if(strncmp(tr[2],"0x",2)==0){
      byteN= (int) strtoul(tr[2],NULL,16);
    }else
      byteN = atoi(tr[2]);
    filLen =atoi(tr[1]);
    p=(char*)strtoul(tr[0],NULL,16);
  }else if(tr[0]!=NULL && tr[1]!=NULL){
    filLen =atoi(tr[1]);
    p=(char*)strtoul(tr[0],NULL,16);
  }else if(tr[0]!=NULL){
    p=(char*)strtoul(tr[0],NULL,16);
  }
  memset(p,byteN,filLen);
}

void doRecursive (int n){
	char autom[MAXLINE];
	static char stat[MAXLINE];
  printf("\n");
	printf ("parameter n:%d en %p\n",n,&n);
	printf ("static array in:%p \n",stat);
	printf ("automatic array in %p\n",autom);
	n--;
	if (n>=0)
		doRecursive(n);
}

void CmdRecursiva (char *tr[]){
	if (tr[0] != NULL)
		doRecursive(atoi(tr[0]));
}

ssize_t WR_Fich (char *fich, void *p, ssize_t n, int override, int read) {
	ssize_t  nleidos,tam=n;
	int df, aux, flags;
	struct stat s;
	s.st_size = 0;
  if(override)
    flags =O_WRONLY | O_RDONLY | O_CREAT | O_TRUNC;
  else
    flags =O_WRONLY | O_RDONLY | O_CREAT | O_EXCL;
  if(read)
    flags =O_WRONLY;
	if ((df=open(fich,flags,0777))==-1 || (stat (fich,&s)==-1))
		return ((ssize_t)-1);
	if (n==LEERCOMPLETO)
		tam=(ssize_t) s.st_size;
	if ((nleidos=write(df,p,tam))==-1){
		aux=errno;
		close(df);
		errno=aux;
    if(!read) printf("%ld bytes written in %s from %p\n",n,fich,p);
		return ((ssize_t)-1);
	}
	if(close (df)!=0) perror("Error closing the file");
	return (nleidos);
}

void CmdRfich (char *tr[]){
  ssize_t size;
  if(tr[0]!=NULL && tr[1]!=NULL && tr[2]!=NULL){
    WR_Fich(tr[0], (char*)strtoul(tr[1],NULL,16), (ssize_t) atoi(tr[2]),0,1);
    printf("%d bytes read of %s in %p\n",atoi(tr[2]),tr[0],tr[1]);
  }else if(tr[0]!=NULL && tr[1]!=NULL){
    if((size=WR_Fich(tr[0], (char*)strtoul(tr[1],NULL,16), LEERCOMPLETO,0,1))!=0)
      perror("Impossible to read file");
    else{
      printf("%ld bytes read of %s in %p\n",size,tr[0],tr[1]);
    }
  }
}

void CmdWfich (char *tr[]){
  if(tr[0]!=NULL && !strcmp(tr[0],"-o") && tr[1]!=NULL && tr[2]!=NULL && tr[3]!=NULL){
    WR_Fich(tr[1], (char*)strtoul(tr[2],NULL,16), (ssize_t) atoi(tr[3]),1,0);
  }else if(tr[0]!=NULL && tr[1]!=NULL && tr[2]!=NULL){
    WR_Fich(tr[0], (char*)strtoul(tr[1],NULL,16), (ssize_t) atoi(tr[2]),0,0);
  }
}

void CmdFork (char *tr[]){
  pid_t pid=fork();
  int status;
  if (pid==-1){
      perror("No se pudo crear el hijo\n");
      exit(-1);
  }else if(pid==0){
      printf("Process created pid:%d\n",getpid());
  }else
      waitpid(pid,&status,0);
}

void CmdPriority (char *tr[]){
	if(tr[0] != NULL){
        pid_t pid;
        pid=getpid();
	    if(tr[1] != NULL){
	        setpriority(PRIO_PROCESS, pid, atoi(tr[1]));
            printf("Priority of process %d: %d\n",pid, getpriority(PRIO_PROCESS, pid));
	    }else
		    printf("Priority of process %d: %d\n",pid, getpriority(PRIO_PROCESS, pid));
	}else
		perror("Not enough information");
}


int setPriority(char *tr){
  int priority;
    tr++;
    priority=atoi(tr);
    return priority;
}

void Foreground(char *tr[],int changepri,int pri){
  pid_t pid;
  int priority=0;
  int status;
  pid=fork();
  if(pid==0){
    if(changepri){
      priority=pri;
      setpriority(PRIO_PROCESS, pid, priority);
      execvp(tr[1],tr+1);
    }else
      execvp(tr[0],tr);
  }else
      waitpid(pid,&status,0);
}

void insertSplano(list l,int priority,pid_t pid,char * pro){
  struct PROC *proc=malloc(sizeof(struct PROC));
  proc->name=strdup(pro);
  proc->priority=priority;
  proc->pid=pid;
  time_t sec = time(&sec); struct tm hour = *localtime(&sec);
  proc->time=hour;
  InsertInList(proc,l);
}

void Background(char *tr[],list l,int changepri,int pri){
  pid_t pid;
  int priority=0;
  pid=fork();
  if(pid==0){
    if(changepri){
      priority=pri;
      setpriority(PRIO_PROCESS, pid, priority);
      execvp(tr[1],tr+1);
    }else
      execvp(tr[0],tr);
  }
  if(changepri){
    insertSplano(l,pri,pid,tr[1]);
  }else
    insertSplano(l,priority,pid,tr[0]);
}

void CmdExec(char *tr[]){
  if(tr[0]!=NULL){
    int priority=0;
    int changepri=0;
    if(!strncmp(tr[0],"@",1)){
        priority=setPriority(tr[0]);
        changepri++;
    }
    if(changepri){
        setpriority(PRIO_PROCESS, getpid(), priority);
        execvp(tr[1],tr+1);
    }else
        execvp(tr[0],tr);
  }
}

void CmdPplano(char *tr[]){
  if(tr[0]!=NULL){
    int priority=0;
    int changepri=0;
    if(!strncmp(tr[0],"@",1)){
        priority=setPriority(tr[0]);
        changepri++;
    }
    Foreground(tr,changepri,priority);
  }
}

void CmdSplano(char *tr[],list l){
  if(tr[0]!=NULL){
    int priority=0;
    int changepri=0;
    if(!strncmp(tr[0],"@",1)){
      priority=setPriority(tr[0]);
      changepri++;
    }
    Background(tr,l,changepri,priority);
  }
}

void CmdProg(char *tr[],list l){
    int i,x = 0;
    int changepri=0;
    int priority=0;
    if(!strncmp(tr[0],"@",1)){
        priority=setPriority(tr[0]);
        changepri++;
    }
    while(tr[x]!=NULL) x++;
    if(!strncmp(tr[x-1],"&",1)){
      char *new[MAXLINE];
      for(i=0; i<x-1;i++)
        new[i]=tr[i];
      Background(new,l,changepri,priority);
    }else{
      Foreground(tr,changepri,priority);
    }
}

void CmdListarProcs(char * tr[],list l){
    PrintListProcs(l);
}

void CmdProc(char * tr[],list l){
    if(tr[0]!=NULL && tr[1]!=NULL){
      PrintListProcs(l);
    }else if(tr[0]!=NULL){
      pid_t pid = atoi(tr[0]);
      PrintProc(l,pid);
    }else{
      printf("aaa\n");
      PrintListProcs(l);
    }
}

void CmdBorrarProcs(char * tr[],list l){
    PrintListProcs(l);
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
    {"hist",CmdHist},
    {"fin",CmdFin},
    {"end",CmdFin},
    {"exit",CmdFin},
    {"crear",CmdCrear},
    {"borrar",CmdBorrar},
    {"info",CmdInfo},
    {"listar",CmdListar},
    {"asignar",CmdAsignar},
    {"desasignar",CmdDesasignar},
    {"mem",CmdMem},
    {"borrarkey",Cmd_borrakey},
    {"rfich",CmdRfich},
    {"wfich",CmdWfich},
    {"recursiva",CmdRecursiva},
    {"llenar",CmdLlenar},
    {"volcar",CmdVolcar},
    {"priority",CmdPriority},
    {"fork",CmdFork},
    {"exec",CmdExec},
    {"pplano",CmdPplano},
    {"splano",CmdSplano},
    {"listarprocs",CmdListarProcs},
    {"proc",CmdProc},
    {"borrarprocs",CmdBorrarProcs},
    {NULL,NULL}
};

void ProcessInput(char *inp, list l){
    char * tr[MAXLINE/2];
    strtok(inp, "\n");
    InsertInArray(inp,strlen(inp));
    if (TrocearCadena(inp,tr)==0)
        return;
    for(int i=0;cmd[i].name!=NULL;i++)
        if(strcmp(tr[0],cmd[i].name)==0){
            (*cmd[i].pfunc)(tr+1,l);
            return;
        }
    CmdProg(tr,l);
}

int main(int argc, char *argv[]){
    char input [MAXLINE];
    int i;

    for(i=0;i<argc;i++)
        printf("main's %dth argument value is:%s\n",i,argv[i]);

    CreateArray();
    CreateMemoryArray();
    list l=CreateList();
    while(1){
        printf("#) ");
        fgets(input,MAXLINE,stdin);
        ProcessInput(input,l);
    }
}
