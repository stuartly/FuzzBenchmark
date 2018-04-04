/*
**  Subsystem:   BeroFTPD Configuration Checker
**  File Name:   ftpck.c               
**                                                        
**  usage: ftpck [ -cFghprstuvx ] [-f accessfile]
**
** This software is Copyright (c) 1997 by Kent Landfield
** BeroFTPD adaption by Bernhard Rosenkraenzer <bero@aachen.linux.de>
**
** Permission is hereby granted to copy, distribute or otherwise 
** use any part of this package as long as you do not try to make 
** money from it or pretend that you wrote it.  This copyright 
** notice must be maintained in any copy made.  If you are interested
** in commercial distribution of this package as a whole or part of
** another commercial package, contact the author for permission.
** If you are a hardware or operating system vendor and would like 
** loan equipment to the author in order to have this ported to your 
** platform, please contact the author so it can be scheduled.
**
** Use of this software constitutes acceptance for use in an AS IS 
** condition. There are NO warranties with regard to this software.  
** In no event shall the author be liable for any damages whatsoever 
** arising out of or in connection with the use or performance of this 
** software.  Any use of this software is at the user's own risk.
**
**  If you make modifications to this software that you feel 
**  increases it usefulness for the rest of the community, please 
**  email the changes, enhancements, bug fixes as well as any and 
**  all ideas to me. This software is going to be maintained and 
**  enhanced as deemed necessary by the community.
**              
**              Kent Landfield
**              kent@landfield.com
**
** History:
**    May 1, 1997     - This is pre-alpha shotware 
**                      Current development platform: Solaris 2.5.1
**
**    May 6, 1997     - Completed the alpha version.
**    June 3, 1997    - Completed the beta version.
**    August 23, 1998 - Adapted to BeroFTPD and added to main BeroFTPD
**                      distribution (Bero)
**
** static char sccsid[] = "@(#)ftpck.c	1.11 06/23/97";
**
**  Files Checked:
**      ftpaccess      - Done (almost)
**      ftpconversions - Done 
**      ftppidnames    - Done
**      ftpgroups      - Done
**      ftpservers     - Done - REDO!!!
**      ftpusers       - Done
**      ftpxferlog     - Done
**      ftphosts       - Done
**
**  Future: do something intelligent with the undocumented guestserver feature
**  Future: complete valid_time function                            
**  Future: verify regexp expression in path-filter
**
**  Future: check any guestgroup accounts specified in passwd file.
**  Future: If shutdown file see if shutdown time earlier than current and 
**          alert user that the server is shutdown.
**   
**  Source Note Acronyms:  
**
**  TBE: To Be Enhanced
**  TBD: To Be Done
*/

#include "config.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include "src/config.h"
#include "pathnames.h"
#include "domains.h"
#include "ftpck.h"

#include <sys/param.h>
#include <netdb.h>

#define MAX_ACCT_LEN 8
#define MAX_ACCESSFILES 1024

#define SEEN           1
#define ADDED          0

#define REWIND         1
#define NOREWIND       0
/*
**   0 == No Errors
** > 0 == Number of errors encountered
** < 0 == Specific error return code
*/
#define PASSED           0
#define FAILED           1
#define MISSING          1

#define NOFTPACCESS      -1            /* File missing      */
#define NOCONVERSIONS    -2            /* File missing      */
#define NOFTPHOSTS       -3            /* File missing      */
#define NOFTPGROUPS      -4            /* File missing      */
#define NOPIDSDIR        -5            /* Directory missing */
#define NOFTPUSERS       -6            /* File missing      */
#define NOXFERLOG        -7            /* File missing      */
#define FTPUSERSPERMS    -8
#define BADFILETYPE      -9
#define BADPERMS        -10
#define PANIC           -11

#ifdef VIRTUAL
#define FTPSERVERSPERMS -13
#define NOFTPSERVERS    -14            /* File missing      */
#endif 

/*
** Used in ftpusers file checking
*/

#define SYSTEM_ACCTS 100

struct user_fnd {
      char *actname;
      int found;
} system_users[SYSTEM_ACCTS];


/*
** Used in file modes checking
*/

struct filetable {
	char *filename;
	mode_t modes;
};

struct filetable fileinfo[] = {
#ifdef VIRTUAL
    {  "ftpservers",       FTPSERVERS_MODES     },
#endif
    {  "ftppid",           FTPPID_MODES         },
    {  "ftpaccess",        FTPACCESS_MODES      },
    {  "ftpconversions",   FTPCONVERSIONS_MODES },
    {  "ftpgroups",        FTPGROUPS_MODES      },
#ifdef HOST_ACCESS
    {  "ftphosts",         FTPHOSTS_MODES       },
#endif
    {  "ftpusers",         FTPUSERS_MODES       },
    {  "xferlog",          XFERLOG_MODES        },
    {  NULL,               0000                 },
    };

/*
** Declarations and tables for ftpaccess line verification
*/

int verify_aliases();
int verify_autogroup();
int verify_banner();
int verify_cdpath();
int verify_compress();
int verify_chmod();
int verify_delete();
int verify_deny();
int verify_email();
int verify_guestgroup();
int verify_guestserver();
int verify_class();
int verify_limit();
int verify_log();
int verify_logfile();
int verify_loginfails();
int verify_lslong();
int verify_lsshort();
int verify_message();
int verify_noretrieve();
int verify_overwrite();
int verify_passwd_check();
int verify_path_filter();
int verify_private();
int verify_readme();
int verify_rename();
int verify_root();
int verify_shutdown();
int verify_tar();
int verify_upload();
int verify_umask();
int verify_virtual();
int verify_NYI();

struct accesstags {
	char *tagname;
	int numparms;    /* minimum number of parms */
	int varparms;    /* 0 - match # -- 1 - variable number of parms */
	int (*verifyfunc)();
};

struct accesstags taglist[] = {
    {  "alias",         2,  0,    verify_aliases      },    /* DONE */
    {  "allow-gid",     1,  1,    verify_NYI          },    /* TBD */
    {  "allow-uid",     1,  1,    verify_NYI          },    /* TBD */
    {  "anonymous-root",2,  1,    verify_NYI          },    /* DONE */
    {  "autogroup",     2,  1,    verify_autogroup    },    /* DONE */
    {  "banner",        1,  0,    verify_banner       },    /* DONE */
    {  "byte-limit",    4,  2,    verify_NYI          },    /* TBD */
    {  "class",         3,  1,    verify_class        },    /* DONE */
    {  "cdpath",        1,  0,    verify_cdpath       },    /* DONE */
    {  "compress",      2,  1,    verify_compress     },    /* DONE */
    {  "chmod",         2,  0,    verify_chmod        },    /* DONE */
    {  "defumask",      2,  1,    verify_NYI          },    /* TBD */
    {  "delete",        2,  0,    verify_delete       },    /* DONE */
    {  "deny",          2,  0,    verify_deny         },    /* DONE */
    {  "deny-email",    1,  1,    verify_NYI          },    /* TBD */
    {  "deny-gid",      1,  1,    verify_NYI          },    /* TBD */
    {  "deny-uid",      1,  1,    verify_NYI          },    /* TBD */
    {  "dl-free",       2,  2,    verify_NYI          },    /* TBD */
    {  "dl-free-dir",   2,  2,    verify_NYI          },    /* TBD */
    {  "email",         1,  0,    verify_email        },    /* DONE */
    {  "file-limit",    4,  2,    verify_NYI          },    /* TBD */
    {  "guest-root",    2,  1,    verify_NYI          },    /* TBD */
    {  "guestgroup",    1,  1,    verify_guestgroup   },    /* DONE */
    {  "guestuser",     1,  1,    verify_NYI          },    /* TBD */
    {  "guestserver",   1,  1,    verify_guestserver  },    /* DONE */
    {  "hostname",      1,  1,    verify_NYI          },    /* TBD */
    {  "include",       1,  1,    verify_NYI          },    /* TBD */
    {  "incmail",       1,  1,    verify_NYI          },    /* TBD */
    {  "limit",         4,  0,    verify_limit        },    /* DONE */
    {  "limit-download",2,  2,    verify_NYI          },    /* TBD */
    {  "limit-time",    2,  2,    verify_NYI          },    /* TBD */
    {  "limit-upload",  2,  2,    verify_NYI          },    /* TBD */
    {  "log",           2,  3,    verify_log          },    /* DONE */
    {  "logfile",       1,  0,    verify_logfile      },    /* DONE */
    {  "loginfails",    1,  0,    verify_loginfails   },    /* DONE */
    {  "lslong",        2,  0,    verify_lslong       },    /* DONE */
    {  "lsshort",       2,  0,    verify_lsshort      },    /* DONE */
    {  "mailfrom",      1,  1,    verify_NYI          },    /* TBD */
    {  "mailserver",    1,  1,    verify_NYI          },    /* TBD */
    {  "message",       2,  1,    verify_message      },    /* DONE */
    {  "nice",          2,  1,    verify_NYI          },    /* TBD */
    {  "noretrieve",    1,  1,    verify_noretrieve   },    /* DONE */
    {  "overwrite",     2,  0,    verify_overwrite    },    /* DONE */
    {  "passive",       4,  3,    verify_NYI          },    /* TBD */
    {  "passwd-check",  3,  0,    verify_passwd_check },    /* DONE */
    {  "path-filter",   5,  0,    verify_path_filter  },    /* DONE */
    {  "private",       1,  0,    verify_private      },    /* DONE */
    {  "readme",        2,  1,    verify_readme       },    /* DONE */
    {  "realgroup",     1,  1,    verify_NYI          },    /* TBD */
    {  "realuser",      1,  1,    verify_NYI          },    /* TBD */
    {  "rename",        2,  0,    verify_rename       },    /* DONE */
    {  "root",          1,  0,    verify_root         },    /* DONE */
    {  "show-everytime",2,  2,    verify_NYI          },    /* TBD */
    {  "shutdown",      1,  0,    verify_shutdown     },    /* DONE */
    {  "tar",           2,  1,    verify_tar          },    /* DONE */
    {  "tcpwindow",     2,  1,    verify_NYI          },    /* TBD */
    {  "throughput",    6,  6,    verify_NYI          },    /* TBD */
    {  "upload",        5,  0,    verify_upload       },    /* DONE */
    {  "ul-dl-rate",    2,  2,    verify_NYI          },    /* TBD */
    {  "umask",         2,  0,    verify_umask        },    /* DONE */
    {  "virtual",       4,  0,    verify_virtual      },    /* DONE */
    {  NULL,            0,  0,    verify_virtual      },
};

/*
** General variable declarations
*/

struct stat sbuf;

char *sp;
char *progname;
char buf[1024];
char *classes[BUFSIZ];
char *accessfiles[BUFSIZ];
char accessfile[BUFSIZ];

/* for paths.c */
char hostname[MAXHOSTNAMELEN];
char logfile[MAXPATHLEN];

int check_rootdir_aliases = CHECK_ROOTDIR_ALIASES;
int check_anonymous_aliases = CHECK_ANONYMOUS_ALIASES;

int check_rootdir_cdpath  = CHECK_ROOTDIR_CDPATH;
int check_anonymous_cdpath  = CHECK_ANONYMOUS_CDPATH;

int num_classes = 0;
int verbose = 0;
int rc = 0;
int describe = 0;
int num_accessfiles = 0;

int isspace();
int isdigit();

char *strerror();

extern int errno;

/***********************************************************************/
/**************** ACCESSFILE READ ROUTINES           *******************/
/***********************************************************************/

#define MAXARGS         50
#define MAXKWLEN        20

struct directive {
    struct directive *next;
    char keyword[MAXKWLEN];
    char *arg[MAXARGS];
    int lineno;
    int numargs;
};

#define MAXUSERS        1024

#define ARG0    entry->arg[0]
#define ARG1    entry->arg[1]
#define ARG2    entry->arg[2]
#define ARG3    entry->arg[3]
#define ARG4    entry->arg[4]
#define ARG5    entry->arg[5]
#define ARG6    entry->arg[6]
#define ARG7    entry->arg[7]
#define ARG8    entry->arg[8]
#define ARG9    entry->arg[9]
#define ARG     entry->arg

#define NUMARGS entry->numargs
#define LINENO  entry->lineno

char *aclbuf = NULL;
char *accessbuf = NULL;
static struct directive *directives;

/*************************************************************************/
/* FUNCTION  : getdirective                                              */
/* PURPOSE   : Retrieve a named entry from the ACL                       */
/* ARGUMENTS : pointer to the keyword and a handle to the acl members    */
/* RETURNS   : pointer to the acl member containing the keyword or NULL  */
/*************************************************************************/

struct directive * getdirective(char *keyword, struct directive **next)
{
    do {
        if (!*next)
            *next = directives;
        else
            *next = (*next)->next;
    } while (*next && strcmp((*next)->keyword, keyword));

    return (*next);
}

/*************************************************************************/
/* FUNCTION  : getroot                                                   */
/* PURPOSE   : Retrieve the root of the ftp data dir - If virtual look   */
/*           : for the "root" directive else the ftp user homedir.       */
/* ARGUMENTS : None                                                      */
/* RETURNS   : pointer to root directive value or NULL if not found      */
/*************************************************************************/

char *getroot()
{
    static char rootbuf[BUFSIZ];
    struct directive *entry = NULL;
    struct passwd *pw;

    /* This is for the new complete virtual support method */

    if (getdirective("root", &entry)) 
        return(ARG0);

    /* This is for the current baselined virtual support method */

    while (getdirective("virtual",&entry)) {
          if (strcasecmp(ARG1,"root") == 0)
              return(ARG2);
    }
    
    /*
    ** This is for the current, non-virtual site that 
    ** DOES support anonymous ftp 
    */

    if ((pw = getpwnam("ftp")) != NULL) 
       strcpy(rootbuf, pw->pw_dir);

    /*
    ** This is for the current, non-virtual site that 
    ** DOES NOT support anonymous ftp 
    */
 
    else 
       strcpy(rootbuf, "/");

    return(rootbuf);
}
/***********************************************************************/
/********************* GENERAL UTILITY ROUTINES ************************/
/***********************************************************************/

/*************************************************************************/
/* FUNCTION  : print_access_file_entry                                   */
/* PURPOSE   : Print out the parsed line                                 */
/* ARGUMENTS : entry structure                                           */
/* RETURNS   : nothing                                                   */
/*************************************************************************/

void print_access_file_entry(struct directive *entry)
{
    register int i;

    if (describe > 1) {
        printf("\n  * line %d: %s", entry->lineno,entry->keyword);
        for (i = 0; i < entry->numargs; i++)
             printf(" %s", entry->arg[i]);
        printf("\n");
    }
}

/*************************************************************************/
/* FUNCTION  : add_accessfile                                            */
/* PURPOSE   : Check if a ftpaccess file has been seen. If not add it to */
/*           : the list of seen accessfiles.                             */
/* ARGUMENTS : path to access file being examined                        */
/* RETURNS   : SEEN if in list and ADDED if not in list but added to it  */
/*************************************************************************/

int add_accessfile(char *path)
{
     /* add an afile structure to the access file list */
     
     int i;
/*     char *strdup(); */

     /* see if the path passed in is one we have seen */

     for (i = 0; i < num_accessfiles; i++) {
          if (strcmp(accessfiles[i],path) == 0)
                  return(SEEN);
     }
     strcpy(accessfile,path);
     accessfiles[num_accessfiles++] = strdup(path);
     return(ADDED);
}

/*************************************************************************/
/* FUNCTION  : blankline                                                 */
/* PURPOSE   : Check if the line passed in is a blank line.              */
/* ARGUMENTS : line to be  examined                                      */
/* RETURNS   : 0 if not blank and 1 if blank                             */
/*************************************************************************/

int blankline(char *line)
{
     register char *cp;

     for (cp = line; *cp ;cp++) {
        if (!isspace(*cp))
            return (0);
     }   
     return(1);
}

/*************************************************************************/
/* FUNCTION  : stat_executable                                           */
/* PURPOSE   : Check if the executable exists                            */
/* ARGUMENTS : pointer to struct directive and directive name            */
/* RETURNS   : PASSED if found, FAILED if not found                      */
/*************************************************************************/

int stat_executable(struct directive *entry, char *tag)
{
    struct stat stbuf;

    if (describe) {
        print_access_file_entry(entry);   
        printf("    - %s: Verify executable exists.\n", tag);
    }

    /*
    ** need to check to see if the executable to be used
    ** to list the directory is in the proper place.
    */

    if ((stat(ARG0, &stbuf)) < 0) {
       fprintf(stderr,"**ERROR: %s: %d: %s: can't find %s\n",accessfile,LINENO,tag,ARG0);
       return(FAILED);
    }
    return(PASSED);
}

/*************************************************************************/
/* FUNCTION  : statfile                                                  */
/* PURPOSE   : Check if the path passed in exists                        */
/* ARGUMENTS : path to be  examined and wu-ftpd system file typename     */
/* RETURNS   : PASSED if found, MISSING if not found                     */
/*************************************************************************/

int statfile(char *path, char *name, char *errtype)
{
  /*
  ** Check to see if path exists in 
  */
  if ((stat(path, &sbuf)) < 0) {
     fprintf(stderr, "%s: %s file %s missing.\n", errtype, name, path);
     return(MISSING);
  }
  return(PASSED);
}

/*************************************************************************/
/* FUNCTION  : filemodes                                                 */
/* PURPOSE   : Check the modes on the path passed in                     */
/* ARGUMENTS : path to be  examined and wu-ftpd system file typename     */
/* RETURNS   : MISSING     - file is not there to stat                   */
/*           : BADFILETYPE - if not regular file                         */
/*           : BADPERMS    - if permissions don't match the table        */
/*           : PASSED      - if permissions do match the table           */
/*           : PANIC       - if file typename not in the table           */
/*************************************************************************/

int filemodes(char *path, char *name)
{
    struct filetable *ct;

    if ((stat(path, &sbuf)) < 0) {
       fprintf(stderr, "**ERROR: %s file %s missing.\n", name, path);
       return(MISSING);
    }
    /*
    ** This function checks modes on files specified.
    */
    if ((sbuf.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr,"**ERROR: %s: %s not a regular file!.\n",name,path);
        return(BADFILETYPE);
    }

    ct = &fileinfo[0];
    while ((ct->filename) != NULL) {
        if (strcmp(name, ct->filename) == 0) {
            if ((sbuf.st_mode & ~S_IFREG) != ct->modes) {
               fprintf(stderr,"**ERROR: %s: %s - Incorrect file modes. Should be %.4o\n",
                        name, path, ct->modes);
               return(BADPERMS);
            }
            return(PASSED);
        }
        ct++;
    }
    fprintf(stderr,"**ERROR: We have a problem Houston... %s not in filetable\n",name);
    return(PANIC);
}

/*************************************************************************/
/* FUNCTION  : valid_ipaddr                                              */
/* PURPOSE   : Called to validate the IP address format                  */
/* ARGUMENTS : IP Address string                                         */
/* RETURNS   : 0 - not valid IP addr || 1 - valid IP addr format         */
/*************************************************************************/

int valid_ipaddr(char *ipaddr)
{
    register char *cp; 
    register char *octet; 
    char addrbuf[BUFSIZ];

    int vrc;
    long octet_val;
 
    strcpy(addrbuf,ipaddr);

    /*
    ** Need to check if an IP address was specified
    */
    cp = addrbuf;
    octet = cp;
    vrc = 0;                 /* Guilty until proven innocent */

    while (*cp) {            /* Must be a '.' or a 0-9 */
        if (*cp == '.') {
            *cp++ = '\0';
             octet_val = atol(octet);
             if (octet_val < 0 || octet_val > 256)  /* Invalid addr range */
                 return(0);
             octet = cp;
             vrc++;
        }
        if ((*cp < '0' || *cp > '9') && *cp != '*')  /* Invalid character */
           return(0);
        else
            cp++;
    }

    /*
    ** WARNING
    ** This may need to be changed for IPV6
    */
    if (vrc != 3 && vrc != 5)    /* Invalid number of octets */
        return(0);

   return(1);
}


/*************************************************************************/
/* FUNCTION  : valid_domain                                              */
/* PURPOSE   : Assure root domain is valid                               */
/* ARGUMENTS : address to be checked.                                    */
/* RETURNS   : 1 - valid root domain -- 0 - invalid root domain          */
/*************************************************************************/

int valid_domain(char *eaddr)
{
   char *np;
   struct co_code *ccptr;

   /*
   ** First check to see if it has a '.' in it.  Not invalid if
   ** there is no '.'.  
   */

   if ((np = strrchr(eaddr,'.')) != NULL) {
        ++np;
        if (!*np)
            return(0);
   }
   else 
        np = eaddr;

   for (ccptr = domain_codes; ccptr->domain != NULL; ccptr++) {
        if (strcasecmp(np, ccptr->domain) == 0)
            return(1);
   }
   return(0);
}

/*************************************************************************/
/* FUNCTION  : match_host                                                */
/* PURPOSE   : Called to validate <addrglob> fields                      */
/* ARGUMENTS : addrglob string (globbed domain name or numeric address   */
/* RETURNS   : PASSED since no current checking is done.                 */
/*************************************************************************/

int match_host(char *hostinfo)
{
   /*
   ** Check to see if this is a valid address for host matching.
   */

   /* 
   ** Is is just a global ?
   */
   if (strcmp(hostinfo,"*") == 0)
       return(PASSED);

   /* 
   ** Has a valid domain been specified 
   */

   if (valid_domain(hostinfo))
       return(PASSED);
 
   /*
   ** brute force checking for IP address 
   */
   if (valid_ipaddr(hostinfo))
       return(PASSED);

   return(FAILED);
}

/*************************************************************************/
/* FUNCTION  : valid_time (TBD)                                          */
/* PURPOSE   : verify the time format passed in is legal                 */
/* ARGUMENTS : time format string                                        */
/* RETURNS   : PASSED if valid - FAILED if invalid                       */
/*************************************************************************/

int valid_time(char *timestr)
{
   /*
   ** This needs to check to see if the time string passed in
   ** is in valid L.sys time format.  Now where's my uucp src...
   */
   return(1);
}

/*************************************************************************/
/* FUNCTION  : valid_class                                               */
/* PURPOSE   : Check class passed to see if it's in the ftpaccess file   */
/* ARGUMENTS : class to validate                                         */
/* RETURNS   : 1 if valid and 0 if invalid class                         */
/*************************************************************************/

int valid_class(char *class)
{
    struct directive *entry = NULL;

    while(getdirective("class", &entry)) {
       if (strcmp(class,ARG0) == 0) 
                return(1);
    }
    return(0);
}

/*************************************************************************/
/* FUNCTION  : valid_num                                                 */
/* PURPOSE   : Check to see if the string passed in is all digits        */
/* ARGUMENTS : string                                                    */
/* RETURNS   : 0 - not all digits : 1 - all digits                       */
/*************************************************************************/

int valid_num(char *num)
{
    register char *cp;

    for (cp = num; *cp; cp++) {
        if (!isdigit(*cp))
            return(0);
    }
    return(1);
}

/*************************************************************************/
/* FUNCTION  : valid_typelist                                            */
/* PURPOSE   : Check to assure anonymous, guest and real are the only    */
/*           : members of the passed in typelist                         */
/* ARGUMENTS : pointer to struct directive, directive name and the line  */
/* RETURNS   : FAILED on error and PASSED if a valid typelist passed in  */
/*************************************************************************/

int valid_typelist(char *list, char *tag, int lineno)
{
    register char *typelist;
    register char *cp;

    if (describe) 
        printf("    - %s: Verifying typelist members are anonymous, guest or real.\n",tag);

    cp = list;

    while (*cp) {
        typelist = cp;
        while (*cp && (*cp != ','))
            ++cp;
        if (*cp == ',') 
            *cp++ = '\0';

        if ((strcmp(typelist,"anonymous") != 0) 
            && (strcmp(typelist,"guest") != 0)
            && (strcmp(typelist,"real") != 0)) {
            fprintf(stderr,"**ERROR: %s: %d: %s: invalid typelist member - \"%s\" specified\n", accessfile, lineno, tag, typelist);
            return(FAILED);
        }
    }
    return(PASSED);
}

/*************************************************************************/
/* FUNCTION  : valid_message_file                                        */
/* PURPOSE   : Check to see if the message file is in both guest and     */
/*           : real ftp directory locations                              */
/* ARGUMENTS : path to file and directive name                           */
/* RETURNS   : FAILED on error and PASSED if a valid message file passed */
/*************************************************************************/

int valid_message_file(char *path, char *tag, int lineno)
{
    char *root;
    char wkbuf[BUFSIZ];
    struct stat stbuf;

    /*
    ** check the system root for path referenced 
    */

    if (describe) 
        printf("    - %s: Verifying message file exists in system root\n",tag);

    if (stat(path,&stbuf) != 0) 
       fprintf(stderr,"**ERROR: %s: %d: %s: can't find %s\n",accessfile,lineno,tag,path);

    /* 
    ** Now check to see if the path is accessible.
    */
    root = getroot();

    /*
    ** check the anonymous root for files referenced in alias
    */

    if (describe) 
        printf("    - %s: Verifying message file exists in anomymous area\n",tag);

    sprintf(wkbuf,"%s%s",root,path);
    if (stat(wkbuf,&stbuf) != 0) {
        fprintf(stderr,"**ERROR: %s: %d: %s: can't find %s\n", 
                       accessfile,lineno,tag,wkbuf);
        return(FAILED);
    }
    return(PASSED);
}

/*************************************************************************/
/* FUNCTION  : cktar_compress                                            */
/* PURPOSE   : Check compress and tar directive entries                  */
/* ARGUMENTS : pointer to struct directive and directive name            */
/* RETURNS   : FAILED on error and PASSED if a valid entry passed in     */
/*************************************************************************/

int cktar_compress(struct directive *entry, char *tag)
{
    int cnt;

    /* 
    ** The contents of this directive should be
    **
    ** compress <yes|no> <classglob> [<classglob> ...]
    **
    **  tar <yes|no> <classglob> [<classglob> ...]
    **    Enables compress or  tar  capabilities  for  any  class
    **    matching  any  of  <classglob>.  The actual conversions
    **    are defined in the external file FTPLIB/ftpconversions.
    **
    ** - Check that there are at least 3 arguments,
    ** - Check the class to assure it is a valid class.
    */

    if (describe) {
        print_access_file_entry(entry);   
        printf("    - %s: Verifying minimum of 2 arguments specified.\n", tag);
    }

    if (NUMARGS < 2) {
        fprintf(stderr,"**ERROR: %s: %d: tag: missing field\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\t%s <yes|no> <classglob> [<classglob> ...]\n",tag);
        return(FAILED);
    }
 
    if (describe) 
        printf("    - %s: Verifying only yes/no specified.\n", tag);

    if ((strcmp(ARG0,"no") != 0) && (strcmp(ARG0,"yes") != 0)) {
        fprintf(stderr,"**ERROR: %s: %d: %s: invalid yes/no specified\n",
                accessfile, LINENO, tag);
        return(FAILED);
    }

    if (describe) 
        printf("    - %s: Verifying valid classes specified.\n", tag);

    for(cnt = 1; cnt < NUMARGS; cnt++) {
        if (!valid_class(entry->arg[cnt])) 
            fprintf(stderr,"**ERROR: %s: %d: %s: invalid class %s\n",
                     accessfile,LINENO,tag,entry->arg[cnt]);
    }
    return(PASSED);
}

/*************************************************************************/
/* FUNCTION  : ckpermcaps                                                */
/* PURPOSE   : Called to check permission capability access file records */
/* ARGUMENTS : pointer to struct directive and directive name            */
/* RETURNS   : FAILED on error and PASSED if a valid entry passed in     */
/*************************************************************************/

int ckpermcaps(struct directive *entry, char *tag)
{
    /*
    ** The contents of this directive should be 
    **  permdirective <yes/no> <typelist>
    **
    ** - Check that there are 3 arguments,
    ** - Check for valid yes/no
    ** - Check the typlist only contains real,guest or anonymous.
    */

    if (describe) {
        print_access_file_entry(entry);   
        printf("    - %s: Verifying 3 arguments specified.\n", tag);
    }

    if (NUMARGS != 2) {
        if (NUMARGS < 2) 
            fprintf(stderr,"**ERROR: %s: %d: %s: missing field\n",accessfile,LINENO,tag);
        else 
            fprintf(stderr,"**ERROR: %s: %d: %s: additional fields\n",accessfile,LINENO,tag);
        if (verbose)
            fprintf(stderr,"\t%s <yes|no> <classglob> [<classglob> ...]\n",tag);
        return(FAILED);
    }
 
    if (describe) 
        printf("    - %s: Verifying yes/no specified.\n", tag);

    if ((strcmp(ARG0,"no") != 0) && (strcmp(ARG0,"yes") != 0)) {
        fprintf(stderr,"**ERROR: %s: %d: %s: invalid yes/no specified\n",
                accessfile, LINENO, tag);
        return(FAILED);
    }

    /* verify the typelist and we are done */

    return(valid_typelist(ARG1, tag, LINENO));
}


/***********************************************************************/
/*********** FTPACCESS FILE RECORD VERIFICATION ROUTINES ***************/
/***********************************************************************/
/***********************************************************************/
/* FUNCTION  : verify_xxxxxxxx                                         */
/* PURPOSE   : Called by ckaccessfile to validate the individual       */
/*           : directive entries.                                      */
/* ARGUMENTS : pointer to struct directive and directive name          */
/* RETURNS   : PASSED/FAILED depending on errors encountered.          */
/***********************************************************************/

int verify_aliases(struct directive *entry)
{
    register char *dir;
    char *root;
    char wkbuf[BUFSIZ];
    struct stat stbuf;

    /*
    ** The contents of this directive should be 
    **  alias <string> <dir>
    **
    ** check the number of arguments == 2 
    ** check the dir is a valid directory 
    */

    if (describe) {
        print_access_file_entry(entry);
        puts("    - alias: verifying number of arguments.");
    }

    if (NUMARGS != 2) {  
        if (NUMARGS < 2)
           fprintf(stderr,"**ERROR: %s: %d: alias: missing field\n", accessfile,LINENO);
        else
           fprintf(stderr,"**ERROR: %s: %d: alias: too many fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\talias <string> <dir>\n");
        return(FAILED);
    }
    
    dir = ARG1;

    /* 
    ** Now check to see if the directory path is accessible.
    */
 
    /*
    ** check the system root for files referenced in alias
    */

    if (check_rootdir_aliases) {
       if (describe) 
           puts("    - alias: assuring directory specified exists in root.");

        if (stat(dir,&stbuf) != 0) 
            fprintf(stderr,"**ERROR: %s: %d: alias: can't find directory %s in root ftp directory\n", accessfile, LINENO, dir);
    }

    if (check_anonymous_aliases) {
       /*
       ** check the anonymous root for files referenced in alias
       */
       if (describe) 
           puts("    - alias: assuring directory specified exists in anonymous area.");

       root = getroot();
       sprintf(wkbuf,"%s%s", root,dir);

       if (stat(wkbuf,&stbuf) != 0) 
          fprintf(stderr,"**ERROR: %s: %d: alias: can't find directory %s in anonymous ftp directory\n", accessfile, LINENO, wkbuf);

    }
    return(PASSED);
}

int verify_autogroup(struct directive *entry)
{
    struct group *grrc;
    int cnt;

    /*
    ** The contents of this directive should be 
    **    autogroup <groupname> <class> [<class> ...]
    **
    ** - Check that there are at least 2 arguments,
    ** - Check the groupname using getgrent(2) and 
    **    assure it is a valid system group.
    */

    if (describe) {
        print_access_file_entry(entry);
        puts("    - autogroup: verifying number of arguments.");
    }

    if (NUMARGS < 2) {
        fprintf(stderr,"**ERROR: %s: %d: autogroup: missing field\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tautogroup <groupname> <class> [<class> ...]\n");
        return(FAILED);
    }

    if (describe) 
        puts("    - autogroup: verifying valid group specified.");

    setgrent();
    grrc = getgrnam(ARG0);
    endgrent();

    if (grrc == NULL) {
        fprintf(stderr,"**ERROR: %s: %d: autogroup: \"%s\" not a system group\n",
                   accessfile, LINENO, ARG0);
        return(FAILED);
    }

    if (describe) 
        puts("    - autogroup: verifying valid classes specified.");

    for(cnt = 1; cnt < NUMARGS; cnt++) {
        if (!valid_class(entry->arg[cnt])) 
            fprintf(stderr,"**ERROR: %s: %d: autogroup: \"%s\" not a valid class\n",
                   accessfile, LINENO, entry->arg[cnt]);
    }
    return(FAILED);
}

int verify_banner(struct directive *entry)
{
    struct stat stbuf;

    /*
    ** - Check to assure there is 1 argument
    ** - Check to assure valid banner file
    */

    if (describe) {
        print_access_file_entry(entry);
        puts("    - banner: verifying only one argument specified.");
    }

    if (NUMARGS != 1) {
        fprintf(stderr,"**ERROR: %s: %d: banner: additional fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tbanner <path>\n");
        return(FAILED);
    }

    /*
    ** check the system root for path referenced 
    */

    if (describe) 
        puts("    - banner: verifying banner message file exists.");

    if (stat(ARG0,&stbuf) != 0) {
       fprintf(stderr,"**ERROR: %s: %d: banner: can't find %s\n",
                   accessfile,LINENO,ARG0);
       return(FAILED);
    }
    return(PASSED);
}

int verify_cdpath(struct directive *entry)
{
    char *root;
    char wkbuf[BUFSIZ];
    struct stat stbuf;

    /*
    ** need to check to assure there is 1 argument
    */

    if (describe) {
        print_access_file_entry(entry);
        puts("    - cdpath: verifying only one argument specified.");
    }

    if (NUMARGS != 1) {
        fprintf(stderr,"**ERROR: %s: %d: cdpath: additional fields\n",accessfile,LINENO);
        return(FAILED);
    }

    if (check_rootdir_cdpath) {
        if (describe) 
            puts("    - cdpath: checking system root for directory specified.");
        /*
        ** check the system root for files referenced
        */
        if (stat(ARG0,&stbuf) != 0) 
            fprintf(stderr,"**ERROR: %s: %d: cdpath: can't find %s\n",
                   accessfile,LINENO,ARG0);
    }

    if (check_anonymous_cdpath) {
        /* 
        ** Now check to see if the path is accessible.
        */
        root = getroot();

        /*
        ** check the anonymous root for files referenced in alias
        */

        sprintf(wkbuf,"%s%s",root,ARG0);

        if (describe) 
            puts("    - cdpath: checking anonymous area for directory specified.");

        if (stat(wkbuf,&stbuf) != 0) {
             fprintf(stderr,"**ERROR: %s: %d: cdpath: can't find %s\n", 
                       accessfile,LINENO,wkbuf);
           if (verbose)
               fprintf(stderr,"\tcdpath <dir>\n");
        }
    }
    return(PASSED);
}

int verify_class(struct directive *entry)
{     
    int cnt;

    /* 
    ** class <class> <typelist> <addrglob> [<addrglob> ...]
    **
    ** - Check to assure a minimum of 3 args are available
    ** - Verify there is a valid typelist specified
    ** - Verify the address globbing passed in.
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - class: assuring minimum of 3 arguments specified.");
    }

    if (NUMARGS < 3) {
        fprintf(stderr,"**ERROR: %s: %d: class: missing field\n",accessfile,LINENO);
        if (verbose)
           fprintf(stderr,"\tclass <class> <typelist> <addrglob> [<addrglob> ...]\n");
       return(FAILED);
    }

    if (valid_typelist(ARG1, "class", LINENO) == FAILED)
       return(FAILED);

    if (describe) 
        puts("    - class: Verifying domain and/or IP address globbing format .");

    for (cnt = 2; cnt < NUMARGS; cnt++) {
        if (match_host(entry->arg[cnt]) == FAILED)
            fprintf(stderr,"**ERROR: %s: %d: class: \"%s\" not a valid addrglob\n",
                   accessfile, LINENO, entry->arg[cnt]);
    }
    return(PASSED);
}

int verify_compress(struct directive *entry)
{
    return(cktar_compress(entry, "compress"));
}

int verify_chmod(struct directive *entry)
{
    /* 
    **  chmod <yes|no> <typelist>
    */
    return(ckpermcaps(entry, "chmod"));
}

int verify_delete(struct directive *entry)
{
    /* 
    **  delete <yes|no> <typelist>
    */
    return(ckpermcaps(entry, "delete"));
}

int verify_deny(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    ** deny <addrglob> <message_file>
    **      Always deny  access  to  host(s)  matching  <addrglob>.
    **      <message_file>   is   displayed.    <addrglob>  may  be
    **      "!nameserved" to deny access to sites without a working
    **      nameserver.
    **
    ** - Check that there are 2 arguments
    ** - Check the to assure the addrglob is valid
    ** - Check the to assure the message file is available
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - deny: Verifying 2 arguments specified.\n");
    }


    if (NUMARGS != 2) {
       if (NUMARGS < 2) 
          fprintf(stderr,"**ERROR: %s: %d: deny: missing field\n",accessfile,LINENO);
       else
          fprintf(stderr,"**ERROR: %s: %d: deny: additional fields\n",accessfile,LINENO);
       if (verbose)
           fprintf(stderr,"\tdeny <addrglob> <message_file>\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - deny: Verifying !nameserved, valid domain and/or IP address globing specified.\n");

    if ((strcmp(ARG0,"!nameserved") != 0) && (match_host(ARG0) == FAILED)) {
        fprintf(stderr,"**ERROR: %s: %d: deny: addrglob\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tdeny <addrglob> <message_file>\n");
        return(FAILED);
    }
    return(valid_message_file(ARG1, "deny", LINENO));
}

int verify_email(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    ** email <name>
    **
    ** - Check that there is 1 argument
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - email: Verifying 1 argument exists.");
    }

    if (NUMARGS != 1) {
       fprintf(stderr,"**ERROR: %s: %d: email: - additional fields\n",accessfile,LINENO);
       if (verbose)
           fprintf(stderr,"\temail <name>\n");
       return(FAILED);
    }
    return(PASSED);
}

int verify_guestgroup(struct directive *entry)
{
    char *groupname;
    int cnt;

    /* 
    ** The contents of this directive should be
    **
    ** guestgroup <groupname> [<groupname> ...]
    **
    ** - Check that there is 1 argument
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - guestgroup: Verifying arguments exists.");
    }

    if (NUMARGS < 1) {
       fprintf(stderr,"**ERROR: %s: %d: guestgroup: invalid syntax - missing field\n",
                   accessfile, LINENO);
        if (verbose)
            fprintf(stderr,"\tguestgroup <groupname> [<groupname> ...]\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - guestgroup: Verifying specified groups are valid system groups.");

    for (cnt = 1; cnt < NUMARGS; cnt++) {

        groupname = entry->arg[cnt];

        /*
        ** Check the groupname using getgrent(2)
        ** and assure it is a valid system group.
        */

        setgrent();   /* open system group file */

        if (getgrnam(groupname) == NULL) {
          fprintf(stderr,"**ERROR: %s: %d: guestgroup: \"%s\" not a system group\n",
                       accessfile, LINENO, groupname);
          if (verbose)
              fprintf(stderr,"\tguestgroup <groupname> [<groupname> ...]\n");
        }
        endgrent();     /* close system group file */
    }
    return(PASSED);
}

int verify_guestserver(struct directive *entry)
{
    /*
    ** The contents of this directive should be
    **
    **  guestserver [<machine1> [<machineN>]]
    **
    ** If a "guestserver" directive is present, anonymous access is 
    ** restricted to the machines listed, usually the machine whose 
    ** CNAME on the current domain is "ftp"...  In otherwords, 
    ** "guestserver" will forbid anonymous access on all machines
    ** while "guestserver ftp inf" will allow anonymous access on
    ** the two machines whose CNAMES are "ftp.enst.fr" and "inf.enst.fr".
    **
    ** If anonymous access is denied on the current machine, the user will 
    ** be asked to use the first machine listed (if any) on the "guestserver"
    ** line instead:
    **
    ** 530- Guest login not allowed on this machine, connect to 
    **       ftp.enst.fr instead.
    **
    ** - Check that there is 1 argument
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - guestserver: Verifying arguments exists.");
    }

    if (NUMARGS < 1) {
       fprintf(stderr,"**ERROR: %s: %d: guestserver: missing field\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tguestserver [<machine1> [<machineN>]]\n");
       return(FAILED);
    }

    /*
    ** TBE: guestserver - assure machines are valid names in the domain.
    */

    return(PASSED);
}

int verify_limit(struct directive *entry)
{
    /*
    ** The contents of this directive should be
    **
    ** limit <class> <n> <times> <message_file>
    **
    **    Limit <class> to <n> users at times <times>, displaying
    **    <message_file>  if  user is denied access.  Limit check
    **    is performed at login time only.  If  multiple  "limit"
    **    commands  can  apply  to the current session, the first
    **    applicable one is used.   Failing  to  define  a  valid
    **    limit,  or  a  limit of -1, is equivalent to unlimited.
    **    <times> is in same format as  the  times  in  the  UUCP
    **    L.sys file.
    **
    ** - Check that there are 4 argument
    ** - Check if valid class specified
    ** - Check <n> is a number
    ** - Valid <time> specified
    ** - Message file exists in guest and real ftp directories
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - limit: Verifying 4 arguments exists.");
    }

    if (NUMARGS != 4) {
      if (NUMARGS < 4) 
         fprintf(stderr,"**ERROR: %s: %d: limit: missing field\n",accessfile,LINENO);
      else
         fprintf(stderr,"**ERROR: %s: %d: limit: additional fields\n",accessfile,LINENO);
      if (verbose)
            fprintf(stderr,"\tlimit <class> <n> <times> <message_file>\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - limit: Verifying valid class specified.");

    if (!valid_class(ARG0)) {
        fprintf(stderr,"**ERROR: %s: %d: limit: \"%s\" not a valid class\n",
                   accessfile, LINENO, ARG0);
        return(FAILED);
    }

    if (describe) 
        puts("    - limit: Verifying number of users specified.");

    if (!valid_num(ARG1)) {
        fprintf(stderr,"**ERROR: %s: %d: limit: \"%s\" not a valid number\n",
                   accessfile, LINENO, ARG1);
        return(FAILED);
    }

    if (describe) 
        puts("    - limit: Verifying valid time string specified.");

    if (!valid_time(ARG2)) {
        fprintf(stderr,"**ERROR: %s: %d: limit: \"%s\" not a valid time format\n",
                   accessfile, LINENO, ARG2);
        return(FAILED);
    }

    return(valid_message_file(ARG3, "limit", LINENO));
}

int verify_log(struct directive *entry)
{
    register char *directions;
    register char *cp;

    /* 
    ** The contents of this directive should be
    **
    **     log commands <typelist>
    **
    **          Enables  logging  of  individual  commands  by   users.
    **          <typelist> is a comma-separated list of any of the key-
    **          words "anonymous", "guest" and "real".  If  the  "real"
    **          keyword  is  included,  logging  will be done for users
    **          using  FTP  to  access  real  accounts,  and   if   the
    **          "anonymous"  keyword  is included logging will done for
    **          users using anonymous FTP.  The "guest" keyword matches
    **          guest access accounts (see "guestgroup" for more infor-
    **          mation).
    **
    **     log transfers <typelist> <directions>
    **
    **          Enables logging of file transfers for  either  real  or
    **          anonymous  FTP  users.   Logging  of  transfers  TO the
    **          server  (incoming)  can  be  enabled  separately   from
    **          transfers  FROM the server (outbound).  <typelist> is a
    **          comma-separated   list   of   any   of   the   keywords
    **          "anonymous", "guest" and "real".  If the "real" keyword
    **          is included, logging will be done for users  using  FTP
    **          to access real accounts, and if the "anonymous" keyword
    **          is included logging will done for users using anonymous
    **          FTP.  The "guest" keyword matches guest access accounts
    **          (see "guestgroup" for more information).   <directions>
    **          is  a  comma-separated  list of any of the two keywords
    **          "inbound" and "outbound", and will  respectively  cause
    **          transfers to be logged for files sent to the server and
    **          sent from the server.
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - log: Verifying line type is commands/transfers.");
    }

    if (strcmp(ARG0,"commands") == 0) {
        if (describe) 
            puts("    - log: Verifying commands line has only 2 arguments.");

        if (NUMARGS != 2) {
           if (NUMARGS < 2)
               fprintf(stderr,"**ERROR: %s: %d: log commands: missing field\n",
                                accessfile,LINENO);
           else
               fprintf(stderr,"**ERROR: %s: %d: log commands: additional fields\n",
                                accessfile,LINENO);
           if (verbose)
               fprintf(stderr,"\tlog commands <typelist>\n");
           return(FAILED);
        }

        /*
        ** check typelist 
        */
        if (valid_typelist(ARG1, "log commands", LINENO) == FAILED)
           return(FAILED);
    }

    else if (strcmp(ARG0,"transfers") == 0) {
        if (describe) 
            puts("    - log: Verifying transfers line has only 3 arguments.");
        if (NUMARGS != 3) {
           if (NUMARGS < 3)
               fprintf(stderr,"**ERROR: %s: %d: log transfers: missing field\n",
                                accessfile,LINENO);
           else
               fprintf(stderr,"**ERROR: %s: %d: log transfers: additional fields\n",
                                accessfile,LINENO);
           if (verbose)
               fprintf(stderr,"\tlog transfers <typelist> <directions>\n");
           return(FAILED);
        }

        /*
        ** check typelist 
        */

        if (valid_typelist(ARG1, "log transfers", LINENO) == FAILED)
           return(FAILED);

        /*
        ** check directions 
        */
        cp = ARG2;
    
        if (describe) 
            puts("    - log: Verifying logging direction as either inbound or outbound.");

        while (*cp) {
            directions = cp;
            while (*cp && (*cp != ','))
                ++cp;
            if (*cp == ',') 
                *cp++ = '\0';
    
            if ((strcmp(directions,"inbound") != 0) 
                && (strcmp(directions,"outbound") != 0)) {
                fprintf(stderr,"**ERROR: %s: %d: log transfers: invalid direction - \"%s\" specified\n", accessfile, LINENO, directions);
                if (verbose) 
                    fprintf(stderr,"\tlog commands <typelist>\n");
                return(FAILED);
            }
        }
    }
    else if (strcmp(ARG0,"security") == 0) { /* TBD */
        return(PASSED);
    }    
    else if (strcmp(ARG0,"syslog") == 0) { /* TBD */
        return(PASSED);
    }
    else {
        fprintf(stderr,"**ERROR: %s: %d: log: Invalid log type. transfers or commands only\n", accessfile,LINENO);
        if (verbose) {
            fprintf(stderr,"\tlog commands <typelist>\n");
            fprintf(stderr,"\t         or\n");
            fprintf(stderr,"\tlog transfers <typelist> <directions>\n");
        }
        return(FAILED);
    }
    return(PASSED);
}

int verify_logfile(struct directive *entry)
{
    struct stat stbuf;

    /* 
    ** The contents of this directive should be
    **
    ** logfile <path>
    **
    ** - Check to assure there is 1 argument
    ** - Check to assure logfile exists at specified location
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - logfile: Verifying only 1 argument specified.");
    }

    if (NUMARGS != 1) {
        fprintf(stderr,"**ERROR: %s: %d: %s: additional fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tlogfile <path>\n");
        return(FAILED);
    }

    if (describe) 
        puts("    - logfile: Verifying logfile specified exists.");

    if (stat(ARG0,&stbuf) != 0) {
        fprintf(stderr,"**ERROR: %s: %d: logfile: can't find %s\n",
                     accessfile,LINENO,ARG0);
        if (verbose)
            fprintf(stderr,"\tlogfile <path>\n");
        return(FAILED);
    }
    return(PASSED);
}


int verify_loginfails(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    ** loginfails <num>
    **
    ** - Check that there is 1 argument
    ** - Check an assure it's digits
    */

    if (describe) {
        print_access_file_entry(entry);
        puts("    - loginfails: Verify only 1 argument specified .");
    }

    if (NUMARGS != 1) {
      fprintf(stderr,"**ERROR: %s: %d: loginfails: additional fields\n",
                  accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\tloginfails <number>\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - loginfails: Verify valid number of failures specified .");

    if (!valid_num(ARG0)) { /* not a digit */
      fprintf(stderr,"**ERROR: %s: %d: loginfails: %s not a number\n",
                  accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\tloginfails <number>\n");
       return(FAILED);
    }
    return(PASSED);
}

int verify_lslong(struct directive *entry)
{
     return(stat_executable(entry, "lslong"));
}

int verify_lsshort(struct directive *entry)
{
     return(stat_executable(entry, "lsshort"));
}

int verify_message(struct directive *entry)
{
    int cnt;

    /* 
    ** The contents of this directive should be
    **
    ** message <path> {<when> {<class> ...}}
    **
    **    Define a file with <path> such that ftpd  will  display
    **    the contents of the file to the user login time or upon
    **    using the change working directory command.  The <when>
    **    parameter  may be "LOGIN" or "CWD=<dir>".  If <when> is
    **    "CWD=<dir>", <dir> specifies the new default  directory
    **    which will trigger the notification.
    **
    **    The optional <class> specification allows  the  message
    **    to  be displayed only to members of a particular class.
    **    More than one class may be specified.
    **
    ** - Check to assure minimum of 2 arguments
    ** - Check the <when> is valid
    ** - Verify any classes listed are valid system classes
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - message: Verify a minimum 2 arguments specified.");
    }

    if (NUMARGS < 2) {
      fprintf(stderr,"**ERROR: %s: %d: message: missing fields\n",
                  accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\tmessage <path> {<when> {<class> ...}\n");
       return(FAILED);
    }

    /*
    ** Can't really check for the file specified by message since it is
    ** only put out if it exists at a point in time (login, cd)
    */

    /* 
    ** check the login/cwd
    */

    if (describe) 
        puts("    - message: Verify valid \'when\' (login/cwd=) specified.");

    if ((strcasecmp(ARG1,"login") != 0) && 
       (strncasecmp(ARG1,"cwd=",4) != 0)) {
       fprintf(stderr,"**ERROR: %s: %d: message: invalid <when> specified - login/cwd= only\n", accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\tmessage <path> {<when> {<class> ...}\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - message: Verify valid classes specified.");

    for(cnt = 2; cnt < NUMARGS; cnt++) {
        if (!valid_class(entry->arg[cnt]))
            fprintf(stderr,"**ERROR: %s: %d: message: \"%s\" not a valid class\n",
                   accessfile, LINENO, entry->arg[cnt]);
    }

    return(PASSED);
}

int verify_noretrieve(entry)
struct directive *entry;
{
    struct stat stbuf;
    int cnt;

    /* 
    ** noretrieve <filename> <filename> ....
    **   Always deny retrieve-ability of  these  files.  If  the
    **   files  are  an absolute path specification (i.e. begins
    **   with '/' character) then only those  files  are  marked
    **   un-gettable,  otherwise  all  files  with  matching the
    **   filename are refused transfer. Example:
    **
    **          noretrieve /etc/passwd core
    **
    **   specifies  no  one  will  be  able  to  get  the   file
    **   /etc/passwd  whereas they will be allowed to transfer a
    **   file `passwd' if it is not in /etc. On the  other  hand
    **   no  one will be able to get files named `core' wherever
    **   it is.
    **
    **   No globbing is done.
    **   
    ** - Check that there is 1 or more arguments
    ** - Check the to assure the <filename> file is available
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - noretrieve: Verify a minimum of 1 argument specified.");
    }

    if (NUMARGS < 1) {
        fprintf(stderr,"**ERROR: %s: %d: noretrieve: missing field\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tnoretrieve <filename> [<filename> ...]\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - noretrieve: Verify specified files exist (not an error if they don\'t.");

    for (cnt = 1; cnt < NUMARGS; cnt++) {
        if (stat(entry->arg[cnt],&stbuf) != 0) {
            fprintf(stderr,"**WARNING: %s: %d: noretrieve: can't find %s\n",
                     accessfile,LINENO,entry->arg[cnt]);
        }
    }
    return(PASSED);
}

int verify_overwrite(struct directive *entry)
{
    /* 
    **  overwrite <yes|no> <typelist>
    */
    return(ckpermcaps(entry, "overwrite"));
}

int verify_passwd_check(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    ** passwd-check <none|trivial|rfc822> (<enforce|warn>) 
    **
    **   Define the level and enforcement of password
    **   checking done by the server for anonymous ftp. 
    **
    **     none      no password checking performed.
    **     trivial   password must contain an '@'.
    **     rfc822    password  must be an rfc822 compliant address.
    **     warn      warn the user, but allow them to log in.
    **     enforce   warn the user, and then log them out.
    **
    ** - Check that there are 2 arguments
    ** - Check the to assure the <filename> file is available
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - passwd-check: Verify 2 arguments specified.");
    }

    if (NUMARGS != 2) {
        if (NUMARGS < 2) 
            fprintf(stderr,"**ERROR: %s: %d: passwd-check: - missing field\n",
                          accessfile,LINENO);
        else
            fprintf(stderr,"**ERROR: %s: %d: passwd-check: - additional fields\n",
                          accessfile,LINENO);
       if (verbose)
           fprintf(stderr,"\tpasswd-check <none|trivial|rfc822> (<enforce|warn>)\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - passwd-check: Verify level of checking (none/rfc822/trivial).");

    if ((strcmp(ARG0,"none") != 0) && 
        (strcmp(ARG0,"rfc822") != 0) && 
        (strcmp(ARG0,"trivial") != 0)) {
        fprintf(stderr,"**ERROR: %s: %d: passwd-check: invalid level - none/trivial/rfc822 only.\n", accessfile, LINENO);
        if (verbose)
            fprintf(stderr,"\tpasswd-check <none|trivial|rfc822> (<enforce|warn>)\n");
       return(FAILED);
    }  

    if (describe) 
        puts("    - passwd-check: Verify type of warning to be issued (warn/enforce).");

    if ((strcmp(ARG1,"enforce") != 0) && 
        (strcmp(ARG1,"warn") != 0)) {
        fprintf(stderr,"**ERROR: %s: %d: passwd-check: invalid enforcement - enforce/warn only.\n", accessfile, LINENO);
        if (verbose)
            fprintf(stderr,"\tpasswd-check <none|trivial|rfc822> (<enforce|warn>)\n");
       return(FAILED);
    }  
    return(PASSED);
}

int verify_path_filter(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    ** path-filter <typelist> <mesg> <allowed_charset> {<disallowed regexp> ...}
    **
    **   For users in <typelist>, path-filter defines regular
    **   expressions that control what a filename can or can
    **   not be. There may be multiple disallowed regexps.
    **   If a filename is invalid due to failure to match the
    **   regexp criteria, <mesg> will be displayed to the
    **   user. For example: 
    **
    **     path-filter anonymous /etc/pathmsg  ^[-A-Za-z0-9._]*$ ^. ^-
    **
    **   specifies that all upload filenames for anonymous
    **   users must be made of only the characters A-Z,
    **   a-z, 0-9, and "._-" and may not begin with a "." or
    **   a "-". If the filename is invalid, /etc/pathmsg will be
    **   displayed to the user. 
    **
    ** - Check to assure minimum of 3 arguments
    ** - Check typelist specified is valid
    ** - Verify message file path exists
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - path-filter: Verify a minimum of 3 arguments specified.");
    }

    if (NUMARGS < 3) {
      fprintf(stderr,"**ERROR: %s: %d: path-filter: missing fields\n",
                  accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\tpath-filter <typelist> <mesg> <allowed_charset> {<disallowed regexp> ...}\n");
       return(FAILED);
    }

    /* Check to assure valid typelist */

    if (valid_typelist(ARG0, "path-filter", LINENO) == FAILED)
       return(FAILED);

    /* Verify message file path exists */

    if (valid_message_file(ARG1, "path-filter", LINENO) == FAILED)
       return(FAILED);

    /* 
    ** TBE: path-filter: verify regular expressions
    ** I'm not even going to try to verify the regexp today... If somebody
    ** person wants to do so and contribute it, I'll gladly put it in.
    */
    return(PASSED);
}

int verify_private(struct directive *entry)
{
    struct stat stbuf;

    /*
    **   private <yes|no>
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - private: Verify only 1 argument specified.");
    }

    if (NUMARGS != 1) {
       fprintf(stderr,"**ERROR: %s: %d: private: - additional fields\n",accessfile,LINENO);
       if (verbose)
           fprintf(stderr,"\tprivate <yes/no>\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - private: Verifying yes/no specified.");

    if ((strcmp(ARG0,"no") != 0) && (strcmp(ARG0,"yes") != 0)) {
        fprintf(stderr,"**ERROR: %s: %d: private: yes/no not specified.\n",
                accessfile, LINENO);
        return(FAILED);
    }

    /*
    ** We need to check to see if the ftpgroups file
    ** exists if the site has specified ftpgroups use.
    */

    if (strcmp(ARG0,"yes") == 0) {

        if (describe) 
            printf("    - private: Verifying if %s file exists.\n",_PATH_PRIVATE);

        if ((stat(_PATH_PRIVATE, &stbuf)) < 0) {
            fprintf(stderr,"**ERROR: %s: %d: private: can't find %s\n",
                accessfile, LINENO, _PATH_PRIVATE);
            return(FAILED);
        }
    }
    return(PASSED);
}

int verify_readme(struct directive *entry)
{
    int cnt;

    /* 
    ** The contents of this directive should be
    **
    ** readme <path> {<when> {<class>}} 
    **
    **   Define a file with <path> such that ftpd will notify
    **   user at login time or upon using the change working
    **   directory command that the file exists and was
    **   modified on such-and-such date. The <when>
    **   parameter may be "LOGIN" or "CWD=<dir>". If
    **   <when> is "CWD=<dir>", <dir> specifies the new
    **   default directory which will trigger the notification.
    **   The message will only be displayed once, to avoid
    **   bothering users. Remember that when README
    **   messages are triggered by an anonymous FTP
    **   user, the <path> must be relative to the base of the
    **   anonymous FTP directory tree. 

    **   The optional <class> specification allows the
    **   message to be displayed only to members of a
    **   particular class. More than one class may be
    **   specified. 
    **
    ** - Check to assure minimum of 2 arguments
    ** - Check the <when> is valid
    ** - Verify any classes listed are valid system classes
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - readme: Verify a minimum of 2 arguments specified.");
    }

    if (NUMARGS < 2) {
      fprintf(stderr,"**ERROR: %s: %d: readme: missing fields\n",
                  accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\treadme <path> {<when> {<class> ...}\n");
       return(FAILED);
    }

    /*
    ** Can't really check for the file specified by readme since it is
    ** only put out if it exists at a point in time (login, cd)
    */

    /* 
    ** check the login/cwd
    */
    if (describe) 
        puts("    - readme: Verify valid \'when\' (login/cwd=) specified.");

    if ((strcasecmp(ARG1,"login") != 0) && 
       (strncasecmp(ARG1,"cwd=",4) != 0)) {
       fprintf(stderr,"**ERROR: %s: %d: readme: invalid <when> specified - login/cwd= only\n", accessfile, LINENO);
       if (verbose)
           fprintf(stderr,"\treadme <path> {<when> {<class> ...}\n");
       return(FAILED);
    }

    for(cnt = 2; cnt < NUMARGS; cnt++) {
        if (!valid_class(entry->arg[cnt]))
            fprintf(stderr,"**ERROR: %s: %d: readme: \"%s\" not a valid class\n",
                   accessfile, LINENO, entry->arg[cnt]);
    }
    return(PASSED);
}

int verify_rename(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    **  rename <yes|no> <typelist>
    */
    return(ckpermcaps(entry, "rename"));
}

int verify_root(struct directive *entry)
{
    struct stat stbuf;

    /* 
    ** The contents of this directive should be
    **
    ** root <dirpath>
    **
    ** - Check to assure there is 1 argument,
    ** - Check to assure path exists at specified location,
    ** - Check to assure path is a directory.
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - root: Verify 1 argument specified.");
    }

    if (NUMARGS != 1) {
        fprintf(stderr,"**ERROR: %s: %d: root: additional fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\troot <path>\n");
        return(FAILED);
    }

    if (describe) 
        puts("    - root: Verify path specified exists and is a directory.");

    if (stat(ARG0,&stbuf) != 0) {
        fprintf(stderr,"**ERROR: %s: %d: root: can't find %s\n",
                     accessfile,LINENO,ARG0);
        if (verbose)
            fprintf(stderr,"\troot <dirpath>\n");
        return(FAILED);
    }

    if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
        fprintf(stderr,"**ERROR: %s: %d: root: can't find %s\n",
                     accessfile,LINENO,ARG0);
        if (verbose)
            fprintf(stderr,"\troot <dirpath>\n");
        return(FAILED);
    }
    return(PASSED);
}

int verify_shutdown(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    **  shutdown <path>
    ** 
    **  TBD: Need to check to see if there are shutdown files on
    **       disk and if the current date is later than the shutdown 
    **       time.  If so put out a Warning letting them know that
    **       the files need to be removed before the server will 
    **       allow connections.
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - shutdown: Verify 1 argument specified.");
    }

    if (NUMARGS != 1) {
        fprintf(stderr,"**ERROR: %s: %d: shutdown: additional fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tshutdown <path>\n");
        return(FAILED);
    }
    return(PASSED);
}

int verify_tar(struct directive *entry)
{
    return(cktar_compress(entry, "tar"));
}

int verify_upload(struct directive *entry)
{
    register char *cp;
    struct passwd *pw;

    /* 
    ** The contents of this directive should be
    **
    **  upload <root-dir> <dirglob> <yes|no> <owner> <group>
    **
    **   Define a directory with <dirglob> that permits or
    **   denies uploads. 
    **
    **   If it does permit uploads, all files will be owned
    **   by <owner> and <group> and will have the
    **   permissions set according to <mode>. 
    **
    **   Directories are matched on a best-match basis. 
    **
    **   For example: 
    **
    **     upload  /var/ftp  *               no
    **     upload  /var/ftp  /incoming       yes   ftp   daemon 0666
    **     upload  /var/ftp  /incoming/gifs  yes   jlc   guest  0600  nodirs
    **
    **   This would only allow uploads into /incoming and
    **   /incoming/gifs. Files that were uploaded to
    **   /incoming would be owned by ftp/daemon and
    **   would have permissions of 0666. File uploaded to
    **   /incoming/gifs would be owned by jlc/guest and
    **   have permissions of 0600. Note that the
    **   <root-dir> here must match the home directory
    **   specified in the password database for the "ftp"
    **   user. 
    **
    **   The optional "dirs" and "nodirs" keywords can
    **   be specified to allow or disallow the creation of
    **   new subdirectories using the mkdir command. 
    **
    **   The upload keyword only applies to users who
    **   have a home directory (the argument to the
    **   chroot() ) of <root-dir>. 
    **
    ** - Check to assure minimum of 3 arguments
    ** - Check to assure maximum of 7 arguments
    ** - Make sure <root-dir> matches the ftp user passwd file homedir 
    ** - Check if ARG2 is yes/no
    ** - Validate dirs/nodirs
    ** - Assure the mode specified is sane
    ** - Assure any specified user has a passwd file entry
    ** - Assure any specified group has a group file entry
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - upload: Verify a minimum of 3 arguments specified.");
    }

    if (NUMARGS < 3) {
        fprintf(stderr,"**ERROR: %s: %d: upload: missing fields\n", accessfile, LINENO);
        if (verbose)
            fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - upload: Verify a maximum of 7 arguments specified.");

    if (NUMARGS > 7) {
        fprintf(stderr,"**ERROR: %s: %d: upload: additional fields\n",accessfile,LINENO);
        if (verbose)
            fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
       return(FAILED);
    }

    if (describe) 
        puts("    - upload: Verify ftp user's homedir area and <root-dir> match");

    /*
    ** Get the location of the ftp user's homedir area and check
    ** to see if it matches the <root-dir>
    */

    if ((pw = getpwnam("ftp")) == NULL) {
         fprintf(stderr,"**ERROR: %s: %d: upload: no ftp user in passwd file - uploads disabled\n", accessfile, LINENO);
         if (verbose)
            fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
       return(FAILED);
    }

    if (strcasecmp(ARG0,pw->pw_dir) != 0) {
         fprintf(stderr,"**ERROR: %s: %d: upload: %s does not match ftp user homedir %s - uploads disabled\n", accessfile, LINENO, ARG0, pw->pw_dir);
         if (verbose)
            fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
        return(FAILED);
    }

    if (describe) 
        puts("    - upload: Verify upload permission is yes or no only.");

    /*
    ** Check if ARG2 is yes/no
    */

    if ((strcasecmp(ARG2,"yes") != 0) && (strcasecmp(ARG2,"no") != 0)) {
       fprintf(stderr,"**ERROR: %s: %d: upload: invalid yes/no specified\n",
                        accessfile,LINENO);
       if (verbose)
            fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
       return(FAILED);
    }

    if (NUMARGS > 3) {
        /*
        ** Author note: Now things start to get confusing.  Taking notes here so 
        ** I can understand my own code later. If you think this comment is for
        ** you, you are mistaken... (End author note)
        **
        **   Looking at the code seems to indicate that the following are valid
        **   ways to specify nodirs...
        **
        **     upload  /var/ftp  *               no  
        **     upload  /var/ftp  *               no    dirs
        **     upload  /var/ftp  *               yes   nodirs
        **     upload  /var/ftp  /incoming/gifs  yes   jlc   guest  0600  nodirs
        **
        */
    
        /* Approach with caution ... */
    
        if ((NUMARGS == 4) || (NUMARGS == 7)) {
            if (NUMARGS == 4)
                cp = ARG3;
            else
                cp = ARG6;

            if (describe) 
                puts("    - upload: Verify directory creation permission (nodirs/dirs).");

            if ((strcasecmp(cp, "nodirs")) && (strcasecmp(cp, "dirs"))) {
                fprintf(stderr,"**ERROR: %s: %d: upload: invalid dir/nodirs specified\n",
                           accessfile,LINENO);
                if (verbose)
                    fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
                return(FAILED);
            }
        }
       
        /* Here it looks as if both user and group are required to be valid */

        if (NUMARGS >= 5) {

            if (describe) 
                puts("    - upload: Verify user/group specified.");

            if ((getpwnam(ARG3)) == NULL) {
                fprintf(stderr,"**ERROR: %s: %d: upload: %s not in passwd file\n",
                          accessfile, LINENO, ARG3);
                if (verbose)
                    fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
                return(FAILED);
            }
    
            if ((getgrnam(ARG4)) == NULL) {
                fprintf(stderr,"**ERROR: %s: %d: upload: %s not in group file\n", 
                          accessfile, LINENO, ARG4);
                if (verbose)
                    fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
                return(FAILED);
            }

           /* get the mode and make sure that it looks sane */

            if (NUMARGS > 5) {
                if (describe) 
                    puts("    - upload: Verify mode specified looks sane.");

                for (cp = ARG5; *cp; cp++) {
                     if (!isdigit(*cp) || (*cp < '0' && *cp > '7')) {
                         fprintf(stderr,"**ERROR: %s: %d: upload: %s - invalid mode\n", 
                                        accessfile, LINENO, ARG5);
                         if (verbose)
                             fprintf(stderr,"\tupload <root-dir> <dirglob> <yes|no> <owner> <group> [<mode> <dirs/nodirs>]\n");
                         return(FAILED);
                     }
                }
    
            }
        }
    }
    return(PASSED);
}

int verify_umask(struct directive *entry)
{
    /* 
    ** The contents of this directive should be
    **
    **  umask <yes|no> <typelist>
    */
    return(ckpermcaps(entry, "umask"));
}

int verify_virtual(struct directive *entry)
{
#ifdef VIRTUAL
    register char *cp;
    struct stat stbuf;
    int addrfmt = 0;

    /* 
    ** The contents of this directive should be
    **
    **  virtual <address> <root|banner|logfile> <path> 
    **
    **   Enables the virtual ftp server capabilities. The <address> is the 
    **   IP address of the virtual server.  The second argument specifies 
    **   that the <path> is either the path to the root of the filesystem 
    **   for this virtual server, the banner presented to the user when
    **   connecting to this virtual server, or the logfile where transfers 
    **   are recorded for this virtual server. If the logfile is not 
    **   specified the default logfile will be used. All other message 
    **   files and permissions as well as any other settings in this file
    **   apply to all virtual servers. 
    **
    ** - Check to assure minimum of 3 arguments
    ** - Make sure <addr> seems sane
    ** - Check if ARG1 is root/banner/logfile
    ** - Assure path exists (only from machine root)
    */

    if (describe) {
        print_access_file_entry(entry);   
        puts("    - virtual: Verify there are 3 arguments.");
    }

    fprintf(stderr,"**WARNING: %s: %d: virtual directive used.\n  This directive is obsolete.\n  Better use the ftpservers file instead.",accessfile,LINENO);
    if (NUMARGS != 3) {
        if (NUMARGS < 3) 
            fprintf(stderr,"**ERROR: %s: %d: virtual: missing fields\n",
                             accessfile,LINENO);
        else
            fprintf(stderr,"**ERROR: %s: %d: virtual: additional fields\n",
                             accessfile,LINENO);
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }

    /* check to assure address specified is sane - no globbing allowed */

    if (describe) 
        puts("    - virtual: Verify IP address format looks reasonable.");

    /* This can be either a hostname or an ipaddress */

    addrfmt = 0; 
    for (cp = ARG0; *cp; cp++) {
        if (!isdigit(*cp) && (*cp != '.')) {
            addrfmt = 1; 
            break;
        }
    }

    if (addrfmt == 0) {   /* Does it look like xxx.xxx.xxx.xxx */
        if (!valid_ipaddr(ARG0)) {
           fprintf(stderr,"**ERROR: %s: %d: virtual: %s - invalid IP address\n",
                                        accessfile, LINENO, ARG0);
           if (verbose)
               fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
           return(FAILED);
        }
    }
    else if (!valid_domain(ARG0)) {
        fprintf(stderr,"**ERROR: %s: %d: virtual: %s - invalid IP address\n",
                                        accessfile, LINENO, ARG0);
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }

    /* check to assure root/banner/logfile line types are valid */

    if (describe) 
        puts("    - virtual: Verify valid line types specified (root/baner/logfile).");

    if ((strcasecmp(ARG1, "root")) && 
        (strcasecmp(ARG1, "banner")) &&
        (strcasecmp(ARG1, "logfile"))) {
        fprintf(stderr,"**ERROR: %s: %d: virtual: invalid line type - root/banner/logfile only\n", accessfile,LINENO); 
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }

    /* verify path exists */

    if (describe) 
        puts("    - virtual: Verify path specified exists and is of the appropriate type.");

    if (stat(ARG2,&stbuf) != 0) {
        fprintf(stderr,"**ERROR: %s: %d: virtual: can't find %s\n",
                          accessfile,LINENO,ARG2);
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }

    if ((!strcasecmp(ARG1,"root")) && ((stbuf.st_mode & S_IFMT) != S_IFDIR)) {
        fprintf(stderr,"**ERROR: %s: %d: virtual: %s - not a directory\n",
                          accessfile,LINENO,ARG2);
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }
    else if ((strcasecmp(ARG1,"root")) && ((stbuf.st_mode & S_IFMT) != S_IFREG)) {
        fprintf(stderr,"**ERROR: %s: %d: virtual: %s - not a regular file\n",
                          accessfile,LINENO,ARG2);
        if (verbose)
           fprintf(stderr,"\tvirtual <address> <root|banner|logfile> <path>\n");
        return(FAILED);
    }

#endif 
    return(PASSED);
}

/* verify_NYI (Not Yet Implemented): Stop ftpck from returning
 * "invalid directive" for directives that are known valid, but not yet
 * supported by ftpck
 */
int verify_NYI(struct directive *entry)
{
    fprintf(stderr,"**WARNING: %s: %d: This directive\n  is known to BeroFTPD, but can't be checked with ftpck yet.\n  You have to verify its correctness yourself.\n",accessfile,LINENO);
    return(PASSED);
}

/***********************************************************************/
/**************** SUPPORT FILE VERIFICATION ROUTINES *******************/
/***********************************************************************/

/***********************************************************************/
/* FUNCTION  : ckaccessfile                                            */
/* PURPOSE   : Validate individual access files. Site may be using the */
/*           : new method of virtual hosting with a separate access    */
/*           : for each virtual domain.                                */
/* ARGUMENTS : path to the access file to be checked                   */
/* RETURNS   : PASSED/FAILED depending on errors encountered.          */
/***********************************************************************/

int ckaccessfile(char *path)
{
    char *ptr;
    char *aclptr;
    char *line;

    int run;
    int cnt;
    int lineno;

    struct stat finfo;
    struct directive *member;
    struct directive *acltail;
    struct accesstags *ct;
    struct directive *np;

    FILE *aclfile;

    char *strerror();

    /*
    ** Check to see if I've seen this access file before. 
    ** No need to check a file twice simply because is is the
    ** compiled in default and is listed in the ftpservers file.
    */

    if (add_accessfile(path) == SEEN) /* store for checking if a duplicate */
        return(PASSED);

    if (verbose)
        printf("\nChecking ftpaccess:           %s\n", path);

    if (describe) {
        puts("    - Verifying modes on the access file.");
        puts("    - Checking for invalid access file directives.");
    }

    /* 
    ** check to see if the access files specified by "path" exists 
    ** and that it's mdes match those in the table.
    */
    filemodes(path, "ftpaccess");

    /* 
    ** This function checks the ftpaccess file passed in. 
    ** Time to get started....
    */

    if ((aclfile = fopen(path, "r")) == NULL) {
        fprintf(stderr,"**ERROR: Can't open access file %s: %s\n",
                        path,strerror(errno));
        return(FAILED);
    }

    if (fstat(fileno(aclfile), &finfo) != 0) {
        fprintf(stderr,"**ERROR: Can't fstat access file %s: %s",
                        path,strerror(errno));
        fclose(aclfile);
        return(FAILED);
    }
    if (finfo.st_size == 0) {
        aclbuf = (char *) calloc(1, 1);
    }
    else {
        if (!(aclbuf = (char *)malloc((unsigned) finfo.st_size + 1))) {
            fprintf(stderr,"**ERROR: Can't malloc aclbuf (%d bytes)",finfo.st_size+1);
            fclose(aclfile);
            return (0);
        }
        if (!fread(aclbuf, (size_t) finfo.st_size, 1, aclfile)) {
            fprintf(stderr,"**ERROR: Error reading access file %s: %s", 
                            path,strerror(errno));
            aclbuf = NULL;
            fclose(aclfile);
            return(FAILED);
        }
        *(aclbuf + finfo.st_size) = '\0';
    
        if (!(accessbuf = (char *)malloc((unsigned) finfo.st_size + 1))) {
            fprintf(stderr,"**ERROR: Can't malloc aclbuf (%d bytes)",finfo.st_size+1);
            fclose(aclfile);
            return(FAILED);
        }
        strcpy(accessbuf, aclbuf);
    }
    fclose(aclfile);

    /*
    ** Now parse it out 
    */

    directives = (struct directive *) NULL;
    acltail = (struct directive *) NULL;

    lineno = 0;

    aclptr = aclbuf;

    while (*aclptr != '\0') {
        line = aclptr;
        while (*aclptr && *aclptr != '\n')
            aclptr++;
        *aclptr++ = (char) NULL;

        lineno++;

        /* deal with comments */
        if ((ptr = strchr(line, '#')) != NULL)
            /* allowed escaped '#' chars for path-filter (DiB) */
            if (*(ptr-1) != '\\')
                *ptr = '\0';

        ptr = strtok(line, " \t");

        if (ptr) {
            member = (struct directive *) calloc(1, sizeof(struct directive));

            strcpy(member->keyword, ptr);
            cnt = 0;
            while ((ptr = strtok(NULL, " \t")) != NULL) {
                if (cnt >= MAXARGS) {
                    fprintf(stderr,
                        "**ERROR: Too many args (>%d) in ftpaccess: %s %s %s %s %s ...",
                        MAXARGS - 1, member->keyword, member->arg[0],
                        member->arg[1], member->arg[2], member->arg[3]);
                    break;
                }
                member->arg[cnt++] = ptr;
            }
            member->numargs = cnt;
            member->lineno = lineno;

            if (acltail)
                acltail->next = member;

            acltail = member;

            if (!directives)
                directives = member;
        }
    }

    /*
    ** Ok... Time to look up the verification routine 
    ** and pass the parmeters found to it to deal with.
    */
   
    np = directives;
    while (np) {
        run = 0;
        ct = &taglist[0];
        while ((ct->tagname) != NULL) {
            if (strcmp(np->keyword, ct->tagname) == 0) {
                /*
                ** We have a match, run the associated verification 
                ** routine for that access file directive type.
                */
                (*ct->verifyfunc)(np);
                run++;
                break;
            }
            ++ct;
        }
        if (!run) 
            fprintf(stderr,"**ERROR: %s: %d: %s: invalid directive\n",
                           accessfile, np->lineno, np->keyword);
        np = np->next;
    }

    if (verbose)
        puts("");

    return(PASSED);
}

/***********************************************************************/
/* FUNCTION  : ckftpcvt                                                */
/* PURPOSE   : Validate compiled in ftp conversions file specified     */
/*           : at the path _PATH_CVT                                   */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOCONVERSIONS   - if file missing                       */
/*           : BADFILETYPE/BADPERMS/PASSED/PANIC - filemode returns    */
/***********************************************************************/

int ckftpcvt(char *path)
{
  FILE *cvtfp;
  char *cp;
  char *root;
  char *external_cmd;         /* command to do conversion */
  char *types;                /* types: {file,directory} OK to convert */
  char *options;              /* for logging: which conversion(s) used */
  char wkbuf[BUFSIZ];
  char cbuf[BUFSIZ];
  int lineno;
  int fcnt;
  struct stat stbuf;

  if (verbose) 
      printf("\nChecking ftpconversions:      %s\n",path);

  if (describe) {
      puts("    - Verifying modes on the ftpconversions file.");
      puts("    - Checking if the ftpconversions file exists.");
      puts("    - Verifying syntax of the ftpconversions file.");
      puts("    - Verifying specified external commands exist in the root.");
      puts("    - Verifying specified external commands exist in the anonymous area.");
      puts("    - Verifying \'types\' specified are valid.");
      puts("    - Verifying \'options\' specified are valid.\n");
  }

  if (statfile(path, "ftpconversions", "ERROR") == MISSING)
     return(NOCONVERSIONS);

  /*
  ** This function needs to check the syntax of the ftp conversion file and
  ** then also check to assure that the progams specified in the file exist
  ** in the proper place and are executable.  This should be examined from
  ** the system, anonymous, and virtual server perspectives.
  **
  ** The conversions known by ftpd(8) and their attributes are stored
  ** in an ASCII file that is structured as below. Each line in the file
  ** provides a description for a single conversion. Fields are
  ** separated by colons (:). 
  **
  **     %s:%s:%s:%s:%s:%s:%s:%s
  **      1  2  3  4  5  6  7  8
  **
  **     Field          Description
  **       1             strip prefix
  **       2             strip postfix
  **       3             addon prefix
  **       4             addon postfix
  **       5             external command
  **       6             types
  **       7             options
  **       8             description
  **
  ** Known Problems
  **
  ** The conversions mechanism does not currently support the strip
  ** prefix and addon prefix fields. 
  */

  filemodes(path, "ftpconversions");

  if ((cvtfp = fopen(path, "r")) == NULL) {
      fprintf(stderr,"**ERROR: Can't open ftpconversion file %s: %s\n", 
             path,strerror(errno));
      return(FAILED);
  }

  lineno = 0;
  while (fgets(cbuf, BUFSIZ, cvtfp) != NULL) {
        lineno++;
        if ((cp = strchr(cbuf, '#')) != NULL)
             *cp = '\0';

        if (blankline(cbuf))
            continue;
 
        /* 
        ** check and make sure there are eight ':' separated fields 
        */

        fcnt = 0;
        
        sp = cbuf;
        cp = sp;
        while (*cp) {
           if (*cp == ':') {
               switch(fcnt) {
                   case 0: break;
                   case 1: break;
                   case 2: break;
                   case 3: break;
                   case 4: external_cmd = sp; break;  /* command to do conversion */
                   case 5: types = sp;        break;  /* types: {file,directory} OK to convert */
                   case 6: options = sp;      break;  /* for logging: which conversion(s) used */
                   case 7: break;
                  default: break;
               }
               *cp = '\0';
               sp = cp+1;
               fcnt++;
           }
           cp++;
        }
    
        if (fcnt != 7) {
            fprintf(stderr,"**ERROR: %s: %d: %s fields\n", 
               path,lineno,(fcnt > 7 ? "too many" : "missing"));
            continue;
        }
        
        /* If here, we have a valid parsed line... 
        **
        ** check for valid external command 
        **     - Strip options off command
        **     - Check the system root for path referenced
        */
        
        cp = external_cmd;
        while (*cp && (!isspace(*cp)))
               cp++;
        if (isspace(*cp))
              *cp = '\0';
        
        if (stat(external_cmd,&stbuf) != 0)
           fprintf(stderr,"**ERROR: %s: %d: can't find %s\n",path,lineno,external_cmd);
 
        root = getroot();
      
        /*
        ** check the anonymous root for files referenced in alias
        */
 
        sprintf(wkbuf,"%s%s",root,external_cmd);
        if (stat(wkbuf,&stbuf) != 0) 
           fprintf(stderr,"**ERROR: %s: %d: can't find %s\n", path,lineno,wkbuf);
    
       /*
       **  Check for valid types 
       */

       for(sp = types, cp = types;*cp;) {
          while (*cp && *cp != '|')
                 cp++;
 
          if (*cp == '|') 
               *cp++ = '\0';
          
          if (strcmp(sp,"T_REG") && strcmp(sp,"T_ASCII") && strcmp(sp,"T_DIR")) 
              fprintf(stderr,"**ERROR: %s: %d: invalid type \'%s\' specified (T_REG|T_ASCII|T_DIR only)\n", path,lineno, sp);
           if (*cp)
               sp = cp;
       }

       /*
       **  Check for valid options 
       */

       if ((strstr(options,"O_COMPRESS") != NULL) && (strstr(options,"O_UNCOMPRESS") != NULL)) {
           fprintf(stderr,"**ERROR: %s: %d: invalid option combination \'%s\'.\n", 
                 path,lineno, options);
           if (verbose)
               fprintf(stderr,"\tCan\'t specify O_COMPRESS and O_UNCOMPRESS together.\n");
       }

       for(sp = options, cp = options;*cp;) {
          while (*cp && *cp != '|')
                 cp++;
 
          if (*cp == '|') 
               *cp++ = '\0';
          
          if (strcmp(sp,"O_COMPRESS") && strcmp(sp,"O_UNCOMPRESS") && strcmp(sp,"O_TAR")) 
              fprintf(stderr,"**ERROR: %s: %d: invalid option \'%s\' specified (O_COMPRESS|O_UNCOMPRESS|O_TAR only)\n", path,lineno, sp);

           if (*cp)
               sp = cp;
       }
  }
  fclose(cvtfp);
  return (PASSED);
}

#ifdef HOST_ACCESS
/***********************************************************************/
/* FUNCTION  : ckftphosts                                              */
/* PURPOSE   : Validate compiled in private file specified at the path */
/*           : _PATH_FTPHOSTS. Only valid for testing if HOST_ACCESS   */
/*           : feature enabled in wu-ftpd config.h                     */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOFTPHOSTS    - if file missing                         */
/*           : BADFILETYPE/BADPERMS/PASSED/PANIC - filemode returns    */
/***********************************************************************/

int ckftphosts(char *path)
{
  int hrc = 0;

#ifdef HOST_ACCESS
  FILE *hfp;
  char *cp;
  char *valp;
  char cbuf[BUFSIZ];
  int lineno;
  int fcnt;

  if (verbose) 
      printf("\nChecking ftphosts:            %s\n",path);

  if (describe) {
      puts("    - Verifying modes on the ftphosts file.");
      puts("    - Checking if the ftphosts file exists.");
      puts("    - Verifying syntax of the ftphosts file.");
      puts("    - Verifying allow/deny keyword usage.");
      puts("    - Verifying valid domain and IP globbing specified.\n");
  }

  if (statfile(path, "ftphosts", "WARNING") == MISSING) {
     fprintf(stderr,"\tOnly needed if using BeroFTPD HOST ACCESS features.\n");
     return(NOFTPHOSTS);
  }

  /*
  ** Now check the syntax of the ftphosts file.
  **
  ** # Example host access file
  ** #
  ** # Everything after a '#' is treated as comment,
  ** # empty lines are ignored
  ** 
  **     allow   bartm   somehost.domain
  **     deny    fred    otherhost.domain 131.211.32.*
  */
  
  filemodes(path, "ftphosts");

  if ((hfp = fopen(path, "r")) == NULL) {
      fprintf(stderr,"**ERROR: Can't open ftphosts file %s: %s\n",
            path,strerror(errno));
      return(FAILED);
  }

  hrc = PASSED;
  lineno = 0;

  while (fgets(cbuf, BUFSIZ, hfp) != NULL) {
      lineno++;
      if ((cp = strchr(cbuf, '#')) != NULL)
           *cp = '\0';
      if ((cp = strchr(cbuf, '\n')) != NULL)
           *cp = '\0';
  
      if (blankline(cbuf))
          continue;

      for (fcnt = 1, cp = cbuf; *cp;) {
          /*
          ** Get permission keyword 
          */
          while(*cp && isspace(*cp)) /* Go to non-blank character */
                cp++;

          if (!*cp)
              break;

          valp = cp; 

          while(*cp && (!isspace(*cp))) /* Go to blank character */
                cp++;

          if (fcnt < 3) {
              if (!*cp) 
                  break;
              *cp++ = '\0';
          }

          if (fcnt == 1) {
             if ((strcmp(valp,"allow") != 0) &&
                  (strcmp(valp, "deny") != 0)) 
                   fprintf(stderr,"**ERROR: %s: %d: %s: invalid access keyword (allow/deny only)\n", path,lineno,valp);
          } 
          else if (fcnt == 3) {
              /*
              ** parse through and validate addresses specified.
              */
        
              for (cp = valp; *cp; ) {
                 while (*cp && (!isspace(*cp)))
                        cp++;
        
                 if (*cp)
                     *cp++ = '\0';
        
                 if (match_host(valp) == FAILED) {
                     fprintf(stderr,"**ERROR: %s: %d: %s: invalid address/domain\n",
                                       path,lineno,valp);
                     hrc = FAILED;
                 }
        
                 while(*cp && isspace(*cp)) /* Go to non-blank character */
                       cp++;
        
                 valp = cp;
              }
          }  
          fcnt++;
      }  

      if (fcnt < 4) 
          fprintf(stderr,"**ERROR: %s: %d: too few fields\n",path,lineno);
  }
#endif 
  return(hrc); 
}
#endif

/***********************************************************************/
/* FUNCTION  : ckftppidnames                                           */
/* PURPOSE   : Validate compiled in pid directory specified at the     */
/*           : path _PATH_PIDNAMES                                     */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOPIDSDIR - if directory missing                        */
/*           : PASSED    - no problems                                 */
/***********************************************************************/

int ckftppidnames()
{
  /*
  ** First check to see if the directory exists in 
  ** the compiled in default location 
  */

  strcpy(buf, _PATH_PIDNAMES);
  sp = (char *)strrchr(buf, '/');
  *sp = '\0';

  if (verbose)
     printf("\nChecking pidfile directory:   %s\n", buf);

  if (describe) 
      puts("    - Making sure the pidfile directory exists.\n");

  if ((stat(buf, &sbuf)) < 0 ) {
       fprintf(stderr,"**ERROR: ftp.pids-* storage directory missing!\n");
       if (verbose) {
           fprintf(stderr,"\tMake this directory [%s] in order for",buf);
           fprintf(stderr,"limit and user count functions to work.\n");
       }
       return(NOPIDSDIR);
  }
  return(PASSED);
}

/***********************************************************************/
/* FUNCTION  : ckftpprivate                                            */
/* PURPOSE   : Validate compiled in private ftpgroups file specified   */
/*           : at the path _PATH_PRIVATE. Not compiled in if the site  */
/*           : has defined NO_PRIVATE in the wu-ftpd config.h file.    */
/* ARGUMENTS : None                                                    */
/* RETURNS   : BADFILETYPE/BADPERMS/PASSED/PANIC - filemode returns    */
/***********************************************************************/

int ckftpprivate(char *path)
{
#ifndef NO_PRIVATE
   FILE *pfp;
   char *cp;
   char *real_grpnam;          /* Real group name   */
   char cbuf[BUFSIZ];
   int lineno;
   int fcnt;
   struct group *getgrnam();
   
   /*
   ** This function needs to check the syntax of the ftpgroups file.
   **
   **    The format of the group access file is: 
   **
   **    access_group_name:encrypted_password:real_group_name
   **
   **    where access_group_name is an arbitrary (alphanumeric + punctuation) 
   **    string.  encrypted_password is the password encrypted via crypt(3), 
   **    exactly like in /etc/passwd.  real_group_name is the name of a valid 
   **    group listed in /etc/group. 
   */

   if (verbose) 
       printf("\nChecking Private file:        %s\n",path);

   if (describe) {
       puts("    - Making sure the ftpgroups file exists.");
       puts("    - Verifying modes on the ftpgroups file.");
       puts("    - Verifying syntax of the ftpgroups file.");
       puts("    - Making sure the groups specified are valid system groups.\n");
   }

   if (statfile(path, "ftpgroups", "WARNING") == MISSING) {
      fprintf(stderr,"\tOnly needed if supporting SITE GROUP and SITE GPASS.\n");
      return(NOFTPGROUPS);
   }

   filemodes(path, "ftpgroups");

   if ((pfp = fopen(path, "r")) == NULL) {
        fprintf(stderr,"**ERROR: Can't open ftpgroups file %s: %s\n", path, strerror(errno));
        return(FAILED);
   }

   lineno = 0;

   while (fgets(cbuf, BUFSIZ, pfp) != NULL) {
        lineno++;
        if ((cp = strchr(cbuf, '#')) != NULL)
             *cp = '\0';

        if (blankline(cbuf))
            continue;
 
        if ((cp = strchr(cbuf, '\n')) != NULL)
             *cp = '\0';

        /*
        ** check and make sure there are two ':' separated fields
        */
 
        fcnt = 0;
        
        sp = cbuf;
        cp = sp;
        while (*cp) {
           if (*cp == ':') {
               *cp++ = '\0';
               switch(fcnt) {
                   case 0: break;
                   case 1: break;
                  default: break;
               }
               sp = cp;
               fcnt++;
           }
           cp++;
        }
    
        if (fcnt != 2) {
            fprintf(stderr,"**ERROR: %s: %d: %s fields\n", path, lineno, (fcnt > 2 ? "too many" : "missing"));
            continue;
        }
        else 
            real_grpnam = sp; 
        
        /*
        ** If here, we have a valid parsed line...
        ** Check and make sure the group is a real system group.
        */

        if (getgrnam(real_grpnam) == NULL)
            fprintf(stderr,"**ERROR: %s: %d: invalid group \'%s\' specified\n", path, lineno, real_grpnam);
   }

   fclose(pfp);
#endif
   return (PASSED);
}

#ifdef VIRTUAL
/***********************************************************************/
/* FUNCTION  : ckftpservers                                            */
/* PURPOSE   : Validate compiled in ftpservers file (support for       */
/*           : complete virtual server support) at _PATH_FTPSERVERS    */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOFTPUSERS - if ftpservers file missing                 */
/*           : FTPSERVERSPERMS - if can't open ftpservers file         */
/*           : number of errors encountered in the specific access     */
/*           : file checking.                                          */
/***********************************************************************/

int ckftpservers(char *path)
{
  int rtc = 0;
  FILE *svrfp;
  char hostaddress[32];
  char accesspath[BUFSIZ];
  char configdir[MAXPATHLEN];
  char cb[BUFSIZ];
  int  read_servers_line();

  if (verbose) 
      printf("\nChecking ftpservers:          %s\n",path);

  if (describe) {
      puts("    - Making sure the ftpservers file exists.");
      puts("    - Verifying modes on the ftpservers file.");
      puts("    - Assuring all ftpaccess files specified exist.");
      puts("    - Individually verifying all ftpaccess files specified.\n");
  }

  if (statfile(path, "ftpservers", "WARNING") == MISSING) {
     fprintf(stderr,"\tOnly needed if you're using virtual hosts.\n");
     return(NOFTPUSERS);
  }
  
  filemodes(path, "ftpservers");

  /* 
  ** Need to check the access files 
  ** specified in the ftpservers file. 
  */

  if ((svrfp = fopen(path, "r")) == NULL) {
      fprintf(stderr,"**ERROR: ftpservers: can't open %s! Check permissions and run %s again.\n", path, progname);
     return(FTPSERVERSPERMS);
  }

  while (read_servers_line(svrfp, hostaddress, configdir, 0) == 1) {
     sp = configdir+(strlen(configdir)-1);
     if (*sp == '/')
         *sp = '\0';
        
     /*
     ** TBD!!! This needs a good deal more... This should...
     ** 
     ** 1st check to see if there is a directory or INTERNAL
     ** If Internal - continue
     ** Determine which configuration files are in the domain directory
     ** If verbose list the files used for a domain
     ** Check them according to the command line options set
     ** back to read the next record.
     */
     sprintf(accesspath,"%s/ftpaccess",configdir);

     if (verbose) {
         sprintf(cb,"%s accessfile:", hostaddress);
         printf("%-30s%s", cb,accesspath);
     }
     if ((stat(accesspath, &sbuf)) < 0)
         fprintf(stderr,"**ERROR: ftpservers: accessfile %s is missing!\n", accesspath);
     else
         rtc += ckaccessfile(accesspath);
  }
  fclose(svrfp);
  return(rtc);
}
#endif

/***********************************************************************/
/* FUNCTION  : ckftpusers                                              */
/* PURPOSE   : Validate ftpusers file                                  */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOFTPUSERS - if ftpusers file missing                   */
/*           : FTPUSERSPERMS - if can't open ftpusers file             */
/*           : number of errors encountered                            */
/***********************************************************************/

int ckftpusers(char *path)
{
  FILE *fp;
  register char *p;
  char line[BUFSIZ];
  char savline[BUFSIZ];
  int cnt;
  int rc;
  int i;
  int num_sysaccts;
  struct passwd *pw;

  if (verbose) 
      printf("\nChecking ftpusers:            %s\n",path);

  if (describe) {
      puts("    - Making sure the ftpusers file exists.");
      puts("    - Verifying modes on the ftpusers file.");
      puts("    - Testing for account names longer than 8 characters.");
      puts("    - Testing to assure only one item specified per line.");
      puts("    - Check individual records newline terminated.");
      puts("    - Check root and all system accounts are in ftpusers file.\n");
  }

  /* 
  ** Check to make sure ftpusers file exists
  ** Check modes on ftpusers file
  **
  ** ftpusers content checking:
  **
  ** - Check to make sure account names are not longer than 8 characters
  ** - Check that there is only one item specified per line.
  ** - Check records newline terminated. (Implementation specific)
  ** - Check system users list and assure they are in ftpusers
  **   (Assure root (or all root equivalents) are in ftpusers file)
  */
  num_sysaccts = 0;

  if (statfile(path, "ftpusers", "WARNING") == MISSING) 
     return(NOFTPUSERS);

  /*
  ** Build a list of system users less than System_accts that excludes
  ** ftp and guest users.
  */

  setpwent();
  while ((pw = getpwent()) != NULL) {
      if (pw->pw_uid < SYSTEM_ACCTS) {
          /*
          ** now check and make sure the FTP accounts and 
          */
          if (strcmp(pw->pw_name,"ftp") == 0) 
              continue;
          /*
          ** guestuser accounts are not included in this list.
          */
          if (strstr(pw->pw_dir,"/./"))
              continue;

          system_users[num_sysaccts].actname = strdup(pw->pw_name);
          system_users[num_sysaccts].found = 0;
          num_sysaccts++;
      }
      else
          break;
  }
  endpwent();

  if ((fp = fopen(path, "r")) == NULL) {
     fprintf(stderr, "**ERROR: ftpusers: Can't open %s! - Check permissions\n", path);
     return(FTPUSERSPERMS);
  }

  cnt=0;
  rc = 0;

  while (fgets(line, sizeof(line), fp) != NULL) {
     cnt++;
     /*
     ** Check for newline as the implementation 
     ** only uses records that are newline terminated 
     */
     if ((p = strchr(line, '\n')) != NULL) {
         *p = '\0';
         if (line[0] == '#')
             continue;
         strcpy(savline, line);
         /*
         ** Check to assure there is only one entry on a line.
         **
         ** trim whitespace off the back 
         */
         --p;
         while (p != line && isspace(*p))
            --p;
         *(p+1) = '\0';

         /* 
         ** trim whitespace off the front 
         */
         p = line; 
         while (*p && isspace(*p))
             ++p;
        
         /*
         ** At this point, if there is whitespace found in the string
         ** then there are more than one item on the line.
         */ 

         sp = p;
         while (*p && !isspace(*p))
             ++p;
         if (isspace(*p)) {
            fprintf(stderr,"**ERROR: %s: %d: multiple entries found: \"%s\"\n", 
                         path,cnt,savline);
            rc++;
             *p = '\0';
         }
         /*
         ** Check for length of any account name specified in the
         ** file to be valid for the system it is running on.
         */
         if (strlen(sp) > MAX_ACCT_LEN) {
            fprintf(stderr,"**ERROR: %s: %d: user \"%s\" greater than %d characters\n",
                       path, cnt, sp, MAX_ACCT_LEN);
            rc++;
         }
         /*
         ** set the found val for the accounts matched.
         */

         for (i = 0; i < num_sysaccts; i++) {
            if (strcmp(sp,system_users[i].actname) == 0) {
                system_users[i].found = 1;
                break;
            }
         }
     }    
     else  {
        fprintf(stderr,"**ERROR: %s: %d: newline missing\n",path,cnt);
        rc++;
     }
  }
  fclose(fp);

  filemodes(path, "ftpusers");

  /* 
  ** Check for missing system accounts and cleanup memory
  */
  for (i = 0; i < num_sysaccts; i++) {
      if (system_users[i].found == 0) {
          fprintf(stderr,"**WARNING: %s: System user \"%s\" not listed\n",
                       path, system_users[i].actname);
          fprintf(stderr,"  This might be a major security problem.\n");
          rc++;
      }
      free(system_users[i].actname);
  }
  return(rc);
}

/***********************************************************************/
/* FUNCTION  : ckftpxferlog                                            */
/* PURPOSE   : Validate compiled in xferlog file specified at the      */
/*           : path _PATH_XFERLOG                                      */
/* ARGUMENTS : None                                                    */
/* RETURNS   : NOXFERLOG - if xferlog file missing                     */
/*           : BADFILETYPE/BADPERMS/PASSED/PANIC - filemode returns    */
/***********************************************************************/

int ckftpxferlog()
{
    if (verbose) 
       printf("\nChecking _PATH_XFERLOG:       %s\n",_PATH_XFERLOG);

    if (describe) {
       puts("    - Testing to assure compiled in value for _PATH_XFERLOG");
       puts("      exists.  It is not a problem if it is missing as it is");
       puts("      created automatically if you specify transfer logging.");
       puts("    - If it exists, the modes on the file are checked as well.\n");
    }

    if (statfile(_PATH_XFERLOG, "xferlog", "WARNING") == MISSING) {
        if (verbose)
           fprintf(stderr,"\tCreated automatically if you do transfer logging.\n");
        return(NOXFERLOG);
    }
    return (filemodes(_PATH_XFERLOG, "xferlog"));
}

/***********************************************************************/
/* FUNCTION  : ckpath_exec()                                           */
/* PURPOSE   : Assure user has not compiled server with _PATH_EXECPATH */
/*           : set to bin.  Security hole if done.                     */
/* ARGUMENTS : None                                                    */
/* RETURNS   : 0 - if compiled correctly                               */
/*           : 1 - otherwise...                                        */
/***********************************************************************/

int ckpath_exec()
{
    if (verbose)
        printf("\nChecking _PATH_EXECPATH:      %s\n",_PATH_EXECPATH);

    if (describe) {
        puts("    - Testing to assure compiled in value for _PATH_EXECPATH is");
        puts("      not set to a standard system directory that may open a");
        puts("      security hole in your system.\n");
    }

    if (!strcmp(_PATH_EXECPATH, "/bin")      || 
        !strcmp(_PATH_EXECPATH, "/bin/")     ||
        !strcmp(_PATH_EXECPATH, "/usr/bin")  || 
        !strcmp(_PATH_EXECPATH, "/usr/bin/") || 
        !strcmp(_PATH_EXECPATH, "/etc")      || 
        !strcmp(_PATH_EXECPATH, "/etc/")     || 
        !strcmp(_PATH_EXECPATH, "/sbin")     || 
        !strcmp(_PATH_EXECPATH, "/sbin/")) {
        fprintf(stderr,"**ERROR: GAK!!!! Security problem - _PATH_EXECPATH == %s !!!!\n",
                              _PATH_EXECPATH);
        fprintf(stderr,"******* Change _PATH_EXECPATH define and recompile NOW!\n");
        return (1);
    }
    return (0);
}

/***********************************************************************/
/* FUNCTION  : ckinetd_conf()                                          */
/* PURPOSE   : Assure user has not specified '-A' to disable the access*/
/*           : file facilities. Otherwise, why are you checking....    */
/* ARGUMENTS : None                                                    */
/* RETURNS   : 0 - if compiled correctly                               */
/*           : 1 - otherwise...                                        */
/***********************************************************************/

int ckinetd_conf(char *path)
{
    int rc;
    int lineno;
    FILE *fp;
    char line[BUFSIZ];
    char *cp;

    if (verbose)
        printf("\nChecking inetd.conf:          %s\n",path);

    if (describe) {
        puts("    - Testing to assure \"-A\" option not present in ftp entry.");
        puts("      (With it, ftpaccess file usage is disabled.)\n");
    }

    if ((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, "**ERROR: %s: Can't open! - Check permissions\n", path);
        return(1);
    }
        
    rc = 0;
    lineno = 0;
    while (fgets(line, BUFSIZ, fp) != NULL) {
         lineno++;
         if (*line == '#')
             continue;

         if (strncmp(line, "ftp", 3) == 0) {
            if ((cp = strstr(line,"-A")) != NULL) {
                fprintf(stderr,"**ERROR: %s: %d: -A option specified, ftpaccess file usage disabled!\n", path, lineno);
                rc = 1;
            }
         }
    }
    fclose(fp);
    return (rc);
}

/***********************************************************************/
/* FUNCTION  : usage                                                   */
/* PURPOSE   : Print the standard usage message so it can be used      */
/* ARGUMENTS : None                                                    */
/* RETURNS   : None                                                    */
/***********************************************************************/

int usage()
{
    printf("\nusage: ftpck [ -ceFghprstuvx ] [-f accessfile]\n");
    printf("\nWith no options, all BeroFTPD configuration files are checked.\n");
    printf("More than one set of checks can be specified at a time.\n");

    printf("\nOptions for checking default config files\n");
    printf("  -c:             Check ftpconversions file at %s\n", _PATH_CVT);
    printf("  -f:             Check ftpaccess file at %s\n", _PATH_FTPACCESS);
    printf("  -g:             Check ftpgroups file at %s\n", _PATH_PRIVATE);
#ifdef HOST_ACCESS
    printf("  -h:             Check ftphosts file at %s\n", _PATH_FTPHOSTS);
#endif
    printf("  -p:             Check pid directory at %s\n", _PATH_PIDNAMES);
#ifdef VIRTUAL
    printf("  -s:             Check ftpservers file at %s\n", _PATH_FTPSERVERS);
#endif
    printf("  -u:             Check ftpusers file at %s\n", _PATH_FTPUSERS);
    printf("  -x:             Check xferlog file at %s\n", _PATH_XFERLOG);

    printf("\nCheck named file options\n");
    printf("  -C conversions: Check the specified ftpconversions file\n");
    printf("  -F ftpaccess:   Check the specified ftpaccess file\n");
    printf("  -G ftpgroups:   Check the specified ftpgroups file\n");
#ifdef HOST_ACCESS
    printf("  -H ftphosts:    Check the specified ftphosts file\n");
#endif
#ifdef VIRTUAL
    printf("  -S ftpservers:  Check the specified ftpservers file\n");
#endif
    printf("  -U ftpusers:    Check the specified ftpusers file\n");

    printf("\nOther options\n");
    printf("  -a:             Verify aliases and cdpaths usable for\n");
    printf("                  anonymous/virtual ftp users\n");
    printf("  -d:             Turn on describe mode. (Very verbose) A\n");
    printf("                  second -d enables accessfile line display\n");
    printf("  -e:             Check _PATH_EXECPATH not == /bin\n");
    printf("  -i:             Check inetd.conf file at %s\n", INETD_CONF);
    printf("  -I inetd.conf:  Check the specified inetd.conf file\n");
    printf("  -r:             Verify aliases and cdpaths usable for real systm users\n");
    printf("  -v:             Produce verbose output\n");
    printf("\n");
    return(1);
}

/***********************************************************************/
/************************** YE OLDE MAIN *******************************/
/***********************************************************************/

int main(int argc,char **argv)
{
   extern char *optarg;
   extern int opterr;

   int getopt();

   char *cptr, *fptr, *gptr, *hptr, *iptr, *sptr, *uptr;
 
   int c;
   int opt = 0;
   int check_path_exec = 0;
   int check_access_file = 0;
   int check_ftpconversions = 0;
#ifdef HOST_ACCESS
   int check_ftphosts = 0;
#endif
   int check_ftpgroups = 0;
#ifdef VIRTUAL
   int check_ftpservers = 0;
#endif
   int check_ftpusers = 0;
   int check_inetd = 0;
   int check_pidnames = 0;
   int check_xferlog = 0;

   opterr = 0;
   progname = argv[0];
   
   /* NEED TO BE ABLE LIMIT THE CHECK TO A SINGLE VIRTUAL DOMAIN"S FILES */

   if (argc > 1) {
     while ((c = getopt(argc, argv, "acdefghiprsuxvC:F:G:H:I:S:U:")) != EOF) {
        opt++;
        switch (c) {
             case 'a': check_anonymous_aliases = 1;
                       check_anonymous_cdpath  = 1; opt--;        break;
             case 'c': check_ftpconversions += 1;                 break;
             case 'C': check_ftpconversions += 2; cptr = optarg;  break;
             case 'd': describe++; verbose++;     opt--;          break;
             case 'e': check_path_exec = 1;                       break;
             case 'f': check_access_file += 1;                    break;
             case 'F': check_access_file += 2;    fptr = optarg;  break;
             case 'g': check_ftpgroups += 1;                      break;
             case 'G': check_ftpgroups += 2;      gptr = optarg;  break;
#ifdef HOST_ACCESS
             case 'h': check_ftphosts += 1;                       break;
             case 'H': check_ftphosts += 2;       hptr = optarg;  break;
#endif
             case 'i': check_inetd += 1;                          break;
             case 'I': check_inetd += 2;          iptr = optarg;  break;
             case 'p': check_pidnames = 1;                        break;
             case 'r': check_rootdir_aliases = 1;
                       check_rootdir_cdpath  = 1; opt--;          break;
#ifdef VIRTUAL
             case 's': check_ftpservers += 1;                     break;
             case 'S': check_ftpservers += 2;     sptr = optarg;  break;
#endif
             case 'u': check_ftpusers += 1;                       break;
             case 'U': check_ftpusers += 2;       uptr = optarg;  break;
             case 'v': verbose++;                 opt--;          break;
             case 'x': check_xferlog = 1;                         break;
             default: return(usage());
        }
     }
  }
  
  if (opt > 0) {
      if (check_path_exec)
          rc += ckpath_exec();

      if (check_inetd == 1 || check_inetd == 3)
          rc += ckinetd_conf(INETD_CONF);
      if (check_inetd > 1)
          rc += ckinetd_conf(iptr);

#ifdef VIRTUAL
      if (check_ftpservers == 1 || check_ftpservers == 3)
          rc += ckftpservers(_PATH_FTPSERVERS);
      if (check_ftpservers > 1)
          rc += ckftpservers(sptr);
#endif 

      if (check_access_file == 1 || check_access_file == 3)
          rc += ckaccessfile(_PATH_FTPACCESS);
      if (check_access_file > 1)
          rc += ckaccessfile(fptr);

      if (check_ftpconversions == 1 || check_ftpconversions == 3)
          rc += ckftpcvt(_PATH_CVT);
      if (check_ftpconversions > 1)
          rc += ckftpcvt(cptr);

#ifdef HOST_ACCESS
      if (check_ftphosts == 1 || check_ftphosts == 3)
          rc += ckftphosts(_PATH_FTPHOSTS);
      if (check_ftphosts > 1)
          rc += ckftphosts(hptr);
#endif

      if (check_ftpgroups == 1 || check_ftpgroups == 3)
          rc += ckftpprivate(_PATH_PRIVATE);
      if (check_ftpgroups > 1)
          rc += ckftpprivate(gptr);

      if (check_ftpusers == 1 || check_ftpusers == 3)
          rc += ckftpusers(_PATH_FTPUSERS);
      if (check_ftpusers > 1)
          rc += ckftpusers(uptr);

      if (check_xferlog == 1)
          rc += ckftpxferlog();

      if (check_pidnames == 1)
          rc += ckftppidnames();
  }
  else { /* If we are here, then let's do it all */
      rc += ckpath_exec();
      rc += ckinetd_conf(INETD_CONF);
#ifdef VIRTUAL
      rc += ckftpservers(_PATH_FTPSERVERS);
#endif
      rc += ckaccessfile(_PATH_FTPACCESS);
      rc += ckftpcvt(_PATH_CVT);
#ifdef HOST_ACCESS
      rc += ckftphosts(_PATH_FTPHOSTS);
#endif
      rc += ckftpprivate(_PATH_PRIVATE);
      rc += ckftpusers(_PATH_FTPUSERS);
      rc += ckftpxferlog();
      rc += ckftppidnames();
  }

  /* 
  ** We're outta here...
  */
  return(rc);
}
