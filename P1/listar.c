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
/*
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
}*/

int main(int argc, char *argv[]){
  int infoList = 0;
  int rec = 0;
  int avoidHid = 0;
    for(int i=1;i<argc;i++){
      if(strcmp(argv[i], "-l") == 0){
        infoList = 1;
      } else if(strcmp(argv[i], "-r") == 0){
        rec= 1;
      } else if(strcmp(argv[i], "-v") == 0){
        avoidHid = 1;
      } else {
        FilterList(argv[i], infoList, rec, avoidHid);
      }
    }
}
