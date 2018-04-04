/*
 *    %W% %G%  -  Locating and opening the appropriate ftpaccess file.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * AUTHOR
 *      Kent Landfield  <kent@landfield.com>
 *
 * HISTORY
 *      970423  KBL      Created
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include "config.h"
#include "pathnames.h"

#ifdef  VIRTUAL

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int virtual_mode=0;
int virtual_ftpaccess=0;
int virtual_len;
struct sockaddr_in virtual_addr;
struct sockaddr_in *virtual_ptr;

#endif

/*
** Pathing storage
*/

#define _PATHS_DEFINED_ 1
#ifdef VIRTUAL
char _path_ftpaccess[MAXPATHLEN];
char _path_ftpusers[MAXPATHLEN];
char _path_ftphosts[MAXPATHLEN];
char _path_private[MAXPATHLEN];
char _path_cvt[MAXPATHLEN];
#endif

extern char logfile[];
extern char hostname[];

int read_servers_line(FILE *svrfp, char *hostaddress, char *accesspath, char resolve);

/* 
** Virtual hosting has to support many different types of needs. There
** must be complete support for the various ftpd system files and their
** functionality. 
**
** Full support on a virtual host basis:
** -------------------------------------
**  _PATH_FTPACCESS  
**  _PATH_FTPUSERS   
**  _PATH_PRIVATE    
**  _PATH_FTPHOSTS   
**  _PATH_CVT        
**
** Set in a site's ftpaccess file
**  _PATH_XFERLOG   
**
** Supported on a site basis:
** --------------------------
**  _PATH_FTPSERVERS 
**  _PATH_EXECPATH   
**  _PATH_PIDNAMES  
**  _PATH_UTMP     
**  _PATH_WTMP    
**  _PATH_LASTLOG  Not used anywhere in the code; removed.
**  _PATH_BSHELL   
**  _PATH_DEVNULL  
*/

/* ------------------------------------------------------------------------ */
/* FUNCTION  : setup_paths                                                  */
/* PURPOSE   : Determine appropriate paths to various configuration files.  */
/* ARGUMENTS : None                                                         */
/* RETURNS   : None                                                         */
/* ------------------------------------------------------------------------ */

void setup_paths()
{
#ifdef VIRTUAL
    char *sp;
    char  configdir[MAXPATHLEN];
    char  filepath[MAXPATHLEN];
    char  hostaddress[32];    
    char  linebuf[BUFSIZ];  
    FILE  *svrfp;     
    struct hostent *shp;
    struct stat st;
#endif

    strcpy(_path_ftpaccess,_PATH_FTPACCESS);
    strcpy(_path_ftpusers, _PATH_FTPUSERS);
#ifdef HOST_ACCESS
    strcpy(_path_ftphosts, _PATH_FTPHOSTS);
#endif
    strcpy(_path_private, _PATH_PRIVATE);
    strcpy(_path_cvt, _PATH_CVT);
    strcpy(logfile, _PATH_XFERLOG);

#ifdef VIRTUAL
    /*
    ** Open PATH_FTPSERVERS config file.  If the file does not 
    ** exist then revert to using the standard _PATH_* path defines.
    */ 

    if ((svrfp = fopen(_PATH_FTPSERVERS, "r")) != NULL) {
        /*
        ** OK.  The ftpservers file exists and is open.
        ** 
        ** Format of the file is:
        **    ipaddr/hostname   directory-containing-configuration-files
        **
        **    208.196.145.10   /etc/ftpd/ftpaccess.somedomain/
        **    208.196.145.200  /etc/ftpd/ftpaccess.someotherdomain/
        **    some.domain      INTERNAL
        ** 
        ** Parse the file and try to match the IP address to one found 
        ** in the file.  If a match is found then return the path to
        ** the specified directory that contains the configuration files
        ** for that specific domain.  If a match is not found, or an invalid
        ** directory path is encountered like above, return standard paths.
        **
        ** As usual, comments and blanklines are ignored.
        */
    
        /* get our address */
    
        virtual_len = sizeof(virtual_addr);
        if (getsockname(0,(struct sockaddr *)&virtual_addr,&virtual_len) == 0) {
            virtual_ptr = (struct sockaddr_in *) &virtual_addr;

            while (read_servers_line(svrfp, hostaddress, configdir, 1) == 1) {
                if (!strcmp(hostaddress, inet_ntoa(virtual_ptr->sin_addr))) {
                    sprintf(linebuf,"VirtualFTP Connect to: %s",
                            inet_ntoa(virtual_ptr->sin_addr));
                    syslog(LOG_NOTICE,"%s",linebuf);
        
                    if (hostname != NULL) {
                        /* reset hostname to this virtual name */
                        shp = gethostbyaddr((char *) &virtual_ptr->sin_addr, sizeof (struct in_addr), AF_INET);
                        if (shp != NULL)
                            strncpy(hostname,shp->h_name,MAXHOSTNAMELEN);
                    }

                    /* 
                    ** check to see that a valid directory value was
                    ** supplied and not something such as "INTERNAL"
                    */

                    if ((stat(configdir,&st) == 0) &&
                        ((st.st_mode & S_IFMT) == S_IFDIR)) {

                        sprintf(filepath,"%s/ftpaccess",configdir);
                        if (access(filepath,R_OK) == 0) {
                            strcpy(_path_ftpaccess,filepath);
                            virtual_mode = 1;
                            virtual_ftpaccess = 1;
                        }
            
                        sprintf(filepath,"%s/ftpusers",configdir);
                        if (access(filepath,R_OK) == 0)
                            strcpy(_path_ftpusers,filepath);
           
                        sprintf(filepath,"%s/ftpgroups",configdir);
                        if (access(filepath,R_OK) == 0)
                            strcpy(_path_private,filepath);
           
                        sprintf(filepath,"%s/ftphosts",configdir);
                        if (access(filepath,R_OK) == 0)
                            strcpy(_path_ftphosts,filepath);
           
                        sprintf(filepath,"%s/ftpconversions",configdir);
                        if (access(filepath,R_OK) == 0)
                            strcpy(_path_cvt,filepath);
                    }
                    return;
                }
            }
        }
    }
#endif  /* VIRTUAL */

    return;
}
