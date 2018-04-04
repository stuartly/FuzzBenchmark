/* ftprestart
**
** removes the ftpd shutdown files.
**
**  In the INSTALL file of wu-ftpd 2.4.* it is recommended to create a link
**  in order for shutdown to work properly for real and anonymous user, e.g.
**  If you use ftpshut, it will create a message file at the location
**  specified in the ftpaccess shutdown directive.
**  ln -s /etc/shutmsg  ~ftp/etc/shutmsg 
**  
**  When ftp service is to be restarted after an ftpshut, the shutdown 
**  message files must be removed. This program reads the ftpaccess
**  file and finds the location of the system shutdown file.  It
**  then proceeds to construct a path to the anonymous ftp area with
**  information found in the "ftp" account.  If virtual ftp servers
**  are enabled, the shutdown message files within those directories 
**  are also removed.
**  
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <pwd.h>

#include "config.h"
#include "pathnames.h"

char *msgfiles[1024];
int numfiles = 0;

int read_servers_line(FILE *, char *, char *, char);
 
int newfile(char *fpath)
{
     int i;
     int fnd;

     /* 
     ** Check to see if the message file path has already been
     ** seen. If so then there is no need to create it again.
     */ 

     fnd = 0;
     for (i = 0; i < numfiles; i++) {
          if (strcmp(msgfiles[i], fpath) == 0) {
              fnd = 1;
              break;
          }
     }  
     if (!fnd) {
         msgfiles[numfiles++] = strdup(fpath);
         return(1);
     }
     return(0);
}

int remove_shutdown_file(char *path)
{
    struct stat stbuf;
    int rc = 1;    /* guilty until proven innocent */

    fprintf(stderr,"ftprestart: %s ",path);

    if (stat(path, &stbuf) == 0) {
        if ((rc = unlink(path)) == 0) 
            fprintf(stderr,"removed.\n");
        else 
            perror(path);
    }
    else
        fprintf(stderr,"does not exist.\n");

    return(rc);
}

int main(int argc, char **argv)
{
    char *p;
    char *cp = NULL;
    char linebuf[BUFSIZ];
    char shutmsg[256];
    char anonpath[BUFSIZ];
    FILE *accessfile;
    struct passwd *pw;

#if defined(VIRTUAL) 
    FILE *svrfp;
    char *sp;
    char hostaddress[32];
    char root[MAXPATHLEN];
    char configdir[MAXPATHLEN];
    char accesspath[MAXPATHLEN];
    char altmsgpath[MAXPATHLEN];
    struct stat finfo;
#endif
 
    if ((accessfile = fopen(_PATH_FTPACCESS, "r")) == NULL) {
       if (errno != ENOENT)
          perror("ftprestart: could not open() access file");
          return(1);
    }
 
    /* 
    ** Search the access file for the 'shutdown' directive.
    */
 
    while (fgets(linebuf,BUFSIZ,accessfile) != NULL) {
       if (strncasecmp(linebuf,"shutdown",8)==0) {
           strtok(linebuf," \t");
           cp=strncpy(shutmsg,strtok(NULL," \t"),sizeof(shutmsg));
           shutmsg[sizeof(shutmsg)-1]='\0';
           if ((p=strchr(cp,'\n'))!=NULL) 
               *p='\0';
       }
    }
 
    if (cp == NULL) {
       fprintf(stderr, "No shutdown file defined in ftpaccess file.\n");
       fclose(accessfile);
       return(1);
    }
 
    msgfiles[numfiles++] = shutmsg;

    /*
    ** Get the location of the anonymous ftp area and check
    ** to see if there is a file shutdown file there as well. 
    ** If so, remove it.
    */
    if ((pw = getpwnam("ftp")) != NULL) {
         sprintf(anonpath,"%s%s",pw->pw_dir,shutmsg);
         if (newfile(anonpath)) 
             remove_shutdown_file(anonpath);
    }

#ifdef VIRTUAL
    /*
    ** Search the access file for virtual ftp servers.
    ** If found, check if there are links/shutdown
    ** message files files in the virtual server areas.
    ** If so, remove them.
    */
            
    rewind(accessfile);

    while (fgets(linebuf,sizeof(linebuf)-1,accessfile) != NULL)  {
       if (strncasecmp(linebuf,"virtual",7)==0) {
           if ((p = strstr(linebuf,"root")) != NULL) {
                p += 4;

               if ((cp = strchr(linebuf,'\n')) != NULL)
                    *cp = '\0';
 
               /* skip to the path */

               while (*p && isspace(*p))
                    p++;
               cp = p;
               while (*p && isalnum(*p))
                    p++;

              sprintf(altmsgpath,"%s%s",cp,shutmsg);
              if (newfile(altmsgpath)) 
                  remove_shutdown_file(altmsgpath);
           }
       }   
    }  


    /*
    ** Need to deal with the access files at the virtual domain directory
    ** locations specified in the ftpservers file.
    */

    if ((svrfp = fopen(_PATH_FTPSERVERS, "r")) != NULL) {
       while (read_servers_line(svrfp, hostaddress, configdir, 0) == 1) {
           /*
           ** check to see that a valid directory value was
           ** supplied and not something such as "INTERNAL"
           **
           ** It is valid to have a string such as "INTERNAL" in the
           ** ftpservers entry. This is not an error. Silently ignore it.
           */

           if ((stat(configdir,&finfo) == 0) &&
               ((finfo.st_mode & S_IFMT) == S_IFDIR))
                sprintf(accesspath,"%s/ftpaccess",configdir);
           else
                continue;

           sprintf(accesspath,"%s/ftpaccess",configdir);

           fclose(accessfile);

           if ((accessfile = fopen(accesspath, "r")) == NULL) {
                if (errno != ENOENT) {
                    fprintf(stderr,"%s: could not open %s accessfile\n",
                              argv[0],accesspath);
                    continue;
                }
           }

           /* need to find the root path */

           while (fgets(linebuf,sizeof(linebuf)-1,accessfile) != NULL)  {
              if ((sp = strstr(linebuf,"root")) != NULL) {
                  if ((cp = strchr(sp,'\n')) != NULL)
                       *cp = '\0';           /* strip newline */
                  sp += 4;                   /* skip past "root" keyword */

                  while (*sp && isspace(*sp)) /* skip whitespace to path */
                        sp++;
                  cp = sp;                   
                  while (*sp && !isspace(*sp)) 
                       sp++;
                  *sp = '\0';           /* truncate blanks, comments etc. */
                  strcpy(root, cp);
                  break;
              }
           }

           rewind(accessfile);

           /* need to find the shutdown message file path */

           while (fgets(linebuf,sizeof(linebuf)-1,accessfile) != NULL)  {
              if ((sp = strstr(linebuf,"shutdown")) != NULL) {
                  if ((cp = strchr(sp,'\n')) != NULL)
                       *cp = '\0';           /* strip newline */
                  sp += 8;                   /* skip past "root" keyword */

                  while (*sp && isspace(*sp)) /* skip whitespace to path */
                        sp++;
                  cp = sp;                   
                  while (*sp && !isspace(*sp)) 
                       sp++;
                  *sp = '\0';           /* truncate blanks, comments etc. */
                  break;
              }
           }

           /*
           ** check to make sure the admin hasn't specified 
           ** a complete path in the 'shutdown' directive.
           */
           if ((sp = strstr(cp,root)) == NULL) 
               sprintf(altmsgpath,"%s%s",root,cp);

           if (newfile(altmsgpath)) 
               remove_shutdown_file(altmsgpath);
       }
       fclose(svrfp);
    }

#endif
    fclose(accessfile);
 
    /*
    ** Time to remove the system wide shutdown file.
    */
    return(remove_shutdown_file(shutmsg));
}
