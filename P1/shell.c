//Autores: Antón Concheiro Fernández anton.concheiro
//         Martín Cabanas Fernández martin.cfernandez

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

#define MAXARRAY 4096
#define MAXLINE 2048
#define MAXNAME 1024

struct COMMAND{
    char *name;
    void (*pfunc) (char *tr[]);
};

//Array implementation and commands
struct POINTERARRAY{
	struct COMMAND *array_of_pointers[MAXARRAY];
	int currentLength;
} arr;

void CreateArray(){
  arr.currentLength = 0;
}

void InsertInArray(struct COMMAND pp){
  arr.array_of_pointers[arr.currentLength]=(struct COMMAND *)malloc(sizeof(struct COMMAND));

	*arr.array_of_pointers[arr.currentLength]=pp;
  arr.currentLength++;
}

void ClearArray(){
  for(int i = 0; i<arr.currentLength;i++){
    free(arr.array_of_pointers[i]);
  }
  arr.currentLength=0;
}

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
        printf("%s\n", arr.array_of_pointers[i]->name);
    }
  }
}

void CmdFin(){
    ClearArray();
    exit(0);
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
    while((dir=readdir(d))!=NULL){//Loops until there is not anything to list
      //struct stat buf;
      //stat(path, &buf);
      printf("%s \n",dir -> d_name/*,buf.st_size*/);//Prints each file or directory
    }//after executing readdir once, it positions the pointer at the next
     //entry, allowing the loop to flow
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
    if(mkdir(tr[1],0755)!=0) /*Creates a dir with the given name and 0755
    permissions and checks if it returns -1 in case of an error*/
      perror("Cannot create directory");
    }
  else if (tr[0]!=NULL){//Check for a name
    FILE *fp;//Creates a pointer fp of type file
    if((fp = fopen(tr[0],"w+"))==NULL)/*Opens a file assigned to fp with the
    given name and w+ permission and checks if NULL in case of an error*/
      perror("Cannot create file");

  }else{
    ListDir(NULL);
  }
}

void CmdMoo(){
  printf("\t\t\t^__^\n");
  printf("\t  _____________/(oo)\n");
  printf("      /\\/(   o       0 /(__)\n");
  printf("\t ( 0\t    o  )  U\n");
  printf("\t (   o  0     o)\n");
  printf("\t | WW─______──||\n");
  printf("\t ||\t      ||\n");
  printf("\t ||           ||\n");
  printf("\t ~~\t      ~~\n");
}

void Rm_Dir(char *path){
   int len = 0;
   DIR *d;/*creates a pointer of type DIR to open the specified directory*/
   d=opendir(path);
   if (d!=NULL){/*if directory could be opened*/
    struct dirent *p;/*creates a dirent struct to access each file name*/
    while ((p=readdir(d))!=NULL){/*traverses inside the directory until
      there are no more files or there was an error (r=-1)*/
      /*skip "." and ".." in the directory*/
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
        continue;
      len = strlen(path) + strlen(p->d_name) + 2;
      /*sets len to the length of the path plus the name of the file plus 2.
      We need 2 because we need to store "/" and the null char*/
      char *buf;/*buffer string to store each file and delete it*/
      buf = malloc(len);
      if (buf!=NULL){/*checks if memory could be allocated*/
         snprintf(buf,len,"%s/%s",path,p->d_name);
         /*stores in buffer variable buf the name of path, followed by "/"
         and followed by the file name, all with length len*/
         struct stat statbuf;/*creates stat struct to check if file
         is a directory*/
         if (stat(buf,&statbuf)==0){/*if 0 it means it was executed
          correctly, meaning buf value is stored in statbuf*/
            if (S_ISDIR(statbuf.st_mode))/*checks if file in statbuf
            is a directory*/
               Rm_Dir(buf);/*recursively call this function in the
               directory*/
            else if(remove(buf)!=0)/*if it is not a directory,
            remove the file and check if it could be removed*/
              perror("Could not delete file");
         }
           free(buf);/*free the pointer buf in memory*/
      }else{
        perror("Lack of memory");/*Error in case of not enough memory*/
      }
    }
    if(closedir(d)!=0){/*closes the opened dir, in case of error prints it*/
      perror("Could not close directory");
    }
  }
  /*try to remove a file, if it is not a file then tries deleting the main dir*/
  if(remove(path)!=0){
    if(rmdir(path)!=0)
      perror("Could not delete directory");
  }
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

void doInfo(char *tr, int shortlist){
  FILE *file;/*struct FILE to get the details of each file*/
  if ((file = fopen(tr, "r"))){/*tries to open the file at pos i*/
    struct stat buf;
    /*creates a struct of type stat to access the permissions of the file*/
    stat(tr, &buf);
    static char str[12];
    /*creates a string to display all the file permissions and type*/
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

    /*get the file name*/
    char* local_file = tr;
    char* ts = strdup(local_file);
    char* filename = basename(ts);

    if(!shortlist){
      struct tm time;
      struct stat stats_link;

      printf("%ld ",buf.st_ino);/*prints inode number*/

      /*prints the string of file permissions*/
      for(int a=0;a<12;a++){
        printf("%c",str[a]);
      }

        /*struct of type passwd to get the username*/
        struct passwd *pwd;
        pwd = getpwuid(buf.st_uid);

        /*struct of type group to get the group name*/
        struct group *grp;
        grp = getgrgid(buf.st_gid);

        time=*(gmtime(&buf.st_mtime));

        /*print the rest of the info*/
        printf(" %ld %s %s %ld %d/%d/%d %d:%d:%d",buf.st_nlink,pwd->pw_name,grp->gr_name,buf.st_size,time.tm_mday,time.tm_mon +1 ,time.tm_year+1900,time.tm_hour,time.tm_min,time.tm_sec);

        if(TipoFichero(buf.st_mode)=='l'){
          stat(tr,&stats_link);
          char *linkName = malloc(stats_link.st_size +1);
          stats_link.st_size=readlink(tr,linkName,stats_link.st_size +1);
          printf(" %s -> %s\n",filename,linkName);
        }else{
          printf(" %s\n",filename);
        }
    }else{
      int size = buf.st_size;
      printf("%s %d\n",filename,size);
    }
    /*closes the file accessed*/
    fclose(file);
  }else{
    /*prints an error in case file could not be opened*/
    perror("Could not open file");
  }
}

void CmdInfo(char *tr[]){
  if(tr[0] != NULL){/*check if there is a name specified*/
    int x;
    for (x = 0; tr[x] != NULL; x++){
      doInfo(tr[x],0);
    }
  }
}

int IsDirectory(char *dir){
    struct stat path_stat;
    stat(dir, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void FilterList(char *path, int infoList, int rec, int avoidHid){
  int len = 0;
  DIR *d;
  struct dirent *p;
  if((d=opendir(path))==NULL){
    perror("Cannot open directory");
    return;
  }
  while((p=readdir(d))!=NULL){
    if(avoidHid==1 && strncmp(p->d_name, ".",1)==0){
      continue;
    }
    len = strlen(path) + strlen(p->d_name) + 2;
    char *buf;/*buffer string to store each file*/
    buf = malloc(len);
    snprintf(buf,len,"%s/%s",path,p->d_name);
    if(infoList==1){
        doInfo(buf,0);
        if(rec && IsDirectory(buf) && strncmp(p->d_name, ".",1)!=0 ){
          printf("%s\n",p->d_name);
          printf("********* %s\n",p->d_name);
          FilterList(buf, infoList, rec, avoidHid);
        }
    }else{
      if(IsDirectory(buf) && strncmp(p->d_name, ".",1)!=0){
        if(rec){
            printf("%s\n",p->d_name);
            printf("********* %s\n",p->d_name);
            FilterList(buf, infoList, rec, avoidHid);
        }else{
          if(infoList || rec || avoidHid)
            printf("%s\n",p->d_name);
          else
            doInfo(p->d_name,1);
        }
      }else{
        if(infoList || rec || avoidHid)
          printf("%s\n",p->d_name);
        else
          doInfo(p->d_name,1);
      }
    }
    free(buf);
  }
  if(closedir(d)!=0)//Closes the opendir, if -1 there is an error
      perror("Error closing directory");
}

void CmdListar(char *tr[]){
    int x = 0;
		int infoList = 0;
    int rec = 0;
    int avoidHid = 0;
    while(tr[x]!=NULL){
		    if(strcmp(tr[x], "-l") == 0){
          infoList = 1;
          x++;
        } else if(strcmp(tr[x], "-r") == 0){
          rec= 1;
          x++;
        } else if(strcmp(tr[x], "-v") == 0){
          avoidHid = 1;
          x++;
        } else {
          break;
        }
    }

   if(tr[x] != NULL){
     while (tr[x] != NULL){
       if (IsDirectory(tr[x])){
              printf("********* %s\n",tr[x]);
              FilterList(tr[x], infoList, rec, avoidHid);
       }else{
         if(infoList){
           doInfo (tr[x],0);
         }else
           doInfo(tr[x],1);
       }
       x++;
     }
   } else{
     printf("********* .\n");
     FilterList(".", infoList, rec, avoidHid);
   }
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
    {"moo",CmdMoo},
    {NULL,NULL}
};

void ProcessInput(char *inp){
    char * tr[MAXLINE/2];

    if (TrocearCadena(inp,tr)==0)
        return;
    for(int i=0;cmd[i].name!=NULL;i++)
        if(strcmp(tr[0],cmd[i].name)==0){
			      InsertInArray(cmd[i]);
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

    CreateArray();
    while(1){
        printf("#) ");
        fgets(input,MAXLINE,stdin);
        ProcessInput(input);
    }
}
