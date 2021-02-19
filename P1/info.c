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

void doInfo(char *tr){
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
    /*closes the file accessed*/
    fclose(file);
  }else{
    /*prints an error in case file could not be opened*/
    perror("Could not open file");
  }
}

int main(int argc, char *argv[]){

    for(int i=1;i<argc;i++)
        doInfo(argv[i]);
}
