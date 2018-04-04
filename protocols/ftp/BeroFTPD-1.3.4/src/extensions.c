/* Copyright (c) 1993, 1994  Washington University in Saint Louis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 3. All advertising
 * materials mentioning features or use of this software must display the
 * following acknowledgement: This product includes software developed by the
 * Washington University in Saint Louis and its contributors. 4. Neither the
 * name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASHINGTON UNIVERSITY AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASHINGTON
 * UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef lint
static char rcsid[] = "@(#)$Id: extensions.c,v 1.1.1.1 1998/08/21 18:10:31 root Exp $";
#endif /* not lint */

#include "config.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#include <pwd.h>
#include <setjmp.h>
#include <grp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>

#if defined(HAVE_SYS_STATVFS_H)
#include <sys/statvfs.h>
#elif defined(HAVE_SYS_STATFS_H)
#include <sys/statfs.h>
#elif defined(HAVE_SYS_VFS_H)
#include <sys/vfs.h>
#endif

#ifdef HAVE_ARPA_FTP_H
#include <arpa/ftp.h>
#else
#include "support/ftp.h"
#endif

#include "pathnames.h"
#include "extensions.h"

#ifdef HAVE_FTW_H
#include <ftw.h>
#else
#include "support/ftw.h"
#endif

#ifdef QUOTA
struct dqblk quota;
char *time_quota(long curstate, long softlimit, long timelimit, char* timeleft);
#endif

#ifdef HAVE_REGEX_H
#include <regex.h>
#endif

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

#ifdef RATIO /* 1998/08/06 K.Wakui */
#define	TRUNC_KB(n)   ((n)/1024+(((n)%1024)?1:0))
extern time_t	login_time;
extern time_t	limit_time;
extern size_t	limit_download;
extern size_t	limit_upload;
extern size_t	total_download;
extern size_t	total_free_dl;
extern size_t	total_upload;
extern int	upload_download_rate;
#endif /* RATIO */
extern int	show_message_everytime;
extern int	show_readme_everytime;

#ifdef OTHER_PASSWD
#include "getpwnam.h"
extern char _path_passwd[];
#endif

extern int fnmatch(),
  type,
  transflag,
  ftwflag,
  authenticated,
  autospout_free,
  data,
  pdata,
  anonymous,
  guest;

#ifdef LOG_FAILED
  extern char the_user[];
#endif

extern char **ftpglob(register char *v),
 *globerr,
  remotehost[],
#ifdef THROUGHPUT
  remoteaddr[],
#endif
  hostname[],
  authuser[],
  chroot_path[],
 *autospout,
  Shutdown[];

char shuttime[30],
  denytime[30],
  disctime[30];

extern char *wu_realpath(const char *pathname, char *result, char* chroot_path);
extern char *fb_realpath(const char *pathname, char *result);

#if !defined(HAVE_REGEX) && !defined(HAVE_REGEXEC)
char *re_comp();
#elif defined(M_UNIX)
extern char *regcmp(), *regex();
#endif

extern FILE *dataconn(char *name, off_t size, char *mode);
FILE *dout;

time_t newer_time;

int show_fullinfo;

int check_newer(char *path, struct stat *st, int flag)
{

    if (st->st_mtime > newer_time) {
        if (show_fullinfo != 0) {
/* This always was a bug, because neither st_size nor time_t were required to
be compatible with int, but needs fixing properly for C9X. */

/* Some systems use one format, some another.  This takes care of the garbage */
#if (defined(BSD) && (BSD >= 199103)) && !defined(LONGOFF_T)
  #define L_FORMAT "qd"
  #define T_FORMAT "d"
#else
#ifdef _AIX42
  #define L_FORMAT "lld"
  #define T_FORMAT "d"
#else
  #define L_FORMAT "d"
  #define T_FORMAT "d"
#endif
#endif

            if (flag == FTW_F || flag == FTW_D) {
                fprintf(dout, "%s %" L_FORMAT " %" T_FORMAT " %s\n", flag == FTW_F ? "F" : "D",
                        st->st_size, st->st_mtime, path);
            }
        } else if (flag == FTW_F)
            fprintf(dout, "%s\n", path);
    }
    /* When an ABOR has been received (which sets ftwflag > 1) return a
     * non-zero value which causes ftw to stop tree traversal and return. */
    return (ftwflag > 1 ? 1 : 0);
}

#if defined(HAVE_SYS_STATVFS_H)
int getSize(char *s)
{
    int c;
    struct statvfs buf;

    if (( c = statvfs(s, &buf)) != 0)
        return(0);

    return(buf.f_bavail * buf.f_frsize / 1024);
}
#elif defined(HAVE_SYS_STATFS_H) || defined(HAVE_SYS_VFS_H)
int getSize(char *s)
{
    int c;
    struct statfs buf;

    if (( c = statfs(s, &buf)) != 0)
        return(0);

    return(buf.f_bavail * buf.f_bsize / 1024);
}
#endif

/*************************************************************************/
/* FUNCTION  : msg_massage                                               */
/* PURPOSE   : Scan a message line for magic cookies, replacing them as  */
/*             needed.                                                   */
/* ARGUMENTS : pointer input and output buffers                          */
/*************************************************************************/

void msg_massage(char *inbuf, char *outbuf)
{
    char *inptr = inbuf;
    char *outptr = outbuf;
#ifdef QUOTA
    char timeleft[80];
#endif
    char buffer[MAXPATHLEN];
    int limit;
    extern struct passwd *pw;
    struct aclmember *entry;
#ifdef VIRTUAL
    extern int virtual_mode;
#ifdef OLDVIRT
    extern char virtual_email[];
#endif
#endif
    time_t curtime;

    acl_getclass(buffer);
    limit = acl_getlimit(buffer, NULL);

    while (*inptr) {
        if (*inptr != '%')
            *outptr++ = *inptr;
        else {
            entry = NULL;
            switch (*++inptr) {
            case 'E':
#if defined(VIRTUAL) && defined(OLDVIRT)
		if (virtual_mode && virtual_email[0] != '\0')
		    sprintf(outptr, "%s", virtual_email);
		else
#endif
                if ( (getaclentry("email", &entry)) && ARG0 )
                    sprintf(outptr, "%s", ARG0); 
                else
                    *outptr = '\0';
                break;
            case 'N': 
                sprintf(outptr, "%d", acl_countusers(buffer)); 
                break; 
            case 'M':
                if (limit == -1)
                    strcpy(outptr,"unlimited");
                else
                    sprintf(outptr, "%d", limit);
                break;
            case 'T':
		time(&curtime);
                strncpy(outptr, ctime(&curtime), 24);
                *(outptr + 24) = '\0';
                break;

            case 'F':
#if defined(HAVE_STATVFS) || defined(HAVE_SYS_VFS_H) || defined(HAVE_SYS_MOUNT)
                sprintf(outptr, "%lu", getSize("."));
#endif
                break;

            case 'C':
#ifdef HAVE_GETCWD
                getcwd(outptr, MAXPATHLEN);
#else
                getwd(outptr);
#endif
                break;

            case 'R':
                strcpy(outptr, remotehost);
                break;

            case 'L':
                strcpy(outptr, hostname);
                break;

            case 'U':
#ifdef LOG_FAILED
                strcpy (outptr, the_user);
#else /* LOG_FAILED */
                strcpy(outptr,
                    (pw == NULL) ? "[unknown]" : pw->pw_name);
#endif /* LOG_FAILED */

                break;

            case 's':
                strncpy(outptr, shuttime, 24);
                *(outptr + 24) = '\0';
                break;

            case 'd':
                strncpy(outptr, disctime, 24);
                *(outptr + 24) = '\0';
                break;

            case 'r':
                strncpy(outptr, denytime, 24);
                *(outptr + 24) = '\0';
                break;

/* KH : cookie %u for RFC931 name */
            case 'u':
                if (authenticated) strncpy(outptr, authuser, 24);
                else strcpy(outptr,"[unknown]");
                *(outptr + 24) = '\0'; 
                break;

#ifdef QUOTA
            case 'B':
#ifdef QUOTA_BLOCKS  /* 1024-blocks instead of 512-blocks */
                sprintf(outptr, "%d", quota.dqb_bhardlimit % 2 ?
                 quota.dqb_bhardlimit / 2 + 1 : quota.dqb_bhardlimit / 2);
#else
                sprintf(outptr, "%d", quota.dqb_bhardlimit);
#endif
                break;

            case 'b':
#ifdef QUOTA_BLOCKS  /* 1024-blocks instead of 512-blocks */
                sprintf(outptr, "%d", quota.dqb_bsoftlimit % 2 ?
                 quota.dqb_bsoftlimit / 2 + 1 : quota.dqb_bsoftlimit / 2);
#else
                sprintf(outptr, "%d", quota.dqb_bsoftlimit);
#endif
                break;

            case 'Q':
#ifdef QUOTA_BLOCKS  /* 1024-blocks instead of 512-blocks */
                sprintf(outptr, "%d", quota.dqb_curblocks % 2 ?
                 quota.dqb_curblocks / 2 + 1 : quota.dqb_curblocks / 2);
#else
                sprintf(outptr, "%d", quota.dqb_curblocks);
#endif
                break;

            case 'I':
#ifdef HAVE_DQBLK_DQB_IHARDLIMIT
                sprintf(outptr, "%d", quota.dqb_ihardlimit);
#else
                sprintf(outptr, "%d", quota.dqb_fhardlimit);
#endif
                break;

            case 'i':
#ifdef HAVE_DQBLK_DQB_ISOFTLIMIT
                sprintf(outptr, "%d", quota.dqb_isoftlimit);
#else
                sprintf(outptr, "%d", quota.dqb_fsoftlimit);
#endif
                break;

            case 'q':
#ifdef HAVE_DQBLK_DQB_CURINODES
                sprintf(outptr, "%d", quota.dqb_curinodes);
#else
                sprintf(outptr, "%d", quota.dqb_curfiles);
#endif
                break;

            case 'H':
                time_quota(quota.dqb_curblocks,quota.dqb_bsoftlimit,
#ifdef HAVE_DQBLK_DQB_BTIME
                               quota.dqb_btime, timeleft);
#else
                               quota.dqb_btimelimit, timeleft);
#endif
                strcpy(outptr, timeleft);
                break;

            case 'h':
#ifdef HAVE_DQBLK_DQB_ISOFTLIMIT
                time_quota(quota.dqb_curinodes,quota.dqb_isoftlimit,
                               quota.dqb_itime, timeleft);
#else
                time_quota(quota.dqb_curfiles,quota.dqb_fsoftlimit,
                               quota.dqb_ftimelimit, timeleft);
#endif
                strcpy(outptr, timeleft);
                break;
#endif

            case '%':
                *outptr++ = '%';
                *outptr = '\0';
                break;
#ifdef RATIO
	    case 'x':
		switch (*++inptr) {
		case 'u':	/* upload bytes */
		    sprintf(outptr,"%d", TRUNC_KB(total_upload) );
		    break;
		case 'd':	/* download bytes */
		    sprintf(outptr,"%d", TRUNC_KB(total_download+total_free_dl) );
		    break;
		case 'R':	/* rate 1:n */
		    if( upload_download_rate > 0 ) {
			sprintf(outptr,"%d", upload_download_rate );
		    }
		    else {
			strcpy(outptr,"free");
		    }
		    break;
		case 'c':	/* credit bytes */
		    if( upload_download_rate > 0 ) {
			size_t credit=total_upload*upload_download_rate-total_download;
			sprintf(outptr,"%d", TRUNC_KB(credit) );
		    }
		    else {
			strcpy(outptr,"unlimited");
		    }
		    break;
		case 'T':	/* time limit (minutes) */
		    if( limit_time > 0 ) {
			sprintf(outptr,"%d", limit_time );
		    }
		    else {
			strcpy(outptr,"unlimited");
		    }
		    break;
		case 'E':	/* elapsed time from loggedin (minutes) */
		    sprintf(outptr,"%d", (time(NULL)-login_time)/60 );
		    break;
		case 'L':	/* times left until force logout (minutes) */
		    if( limit_time > 0 ) {
			sprintf(outptr,"%d", limit_time-(time(NULL)-login_time)/60 );
		    }
		    else {
			strcpy(outptr,"unlimited");
		    }
		    break;
		case 'U':	/* upload limit */
		    if( limit_upload > 0 ) {
			sprintf(outptr,"%d", TRUNC_KB(limit_upload));
		    }
		    else {
			strcpy(outptr,"unlimited");
		    }
		    break;
		case 'D':	/* download limit */
		    if( limit_download > 0 ) {
			sprintf(outptr,"%d",TRUNC_KB(limit_download));
		    }
		    else {
			strcpy(outptr,"unlimited");
		    }
		    break;
		default:
		    strcpy(outptr,"%??");
		    break;
		}
		break;
#endif /* RATIO */

            default:
                *outptr++ = '%';
                *outptr++ = '?';
                *outptr = '\0';
                break;
            }
            while (*outptr)
                outptr++;
        }
        inptr++;
    }
    *outptr = '\0';
}

/*************************************************************************/
/* FUNCTION  : cwd_beenhere                                              */
/* PURPOSE   : Return 1 if the user has already visited this directory   */
/*             via C_WD.                                                 */
/* ARGUMENTS : a power-of-two directory function code (README, MESSAGE)  */
/*************************************************************************/

int cwd_beenhere(int dircode)
{
    struct dirlist {
        struct dirlist *next;
        int dircode;
        char dirname[1];
    };

    static struct dirlist *head = NULL;
    struct dirlist *curptr;
    char cwd[MAXPATHLEN];

    fb_realpath(".", cwd);

    for (curptr = head; curptr != NULL; curptr = curptr->next)
        if (strcmp(curptr->dirname, cwd) == 0) {
            if (!(curptr->dircode & dircode)) {
                curptr->dircode |= dircode;
                return (0);
            }
            return (1);
        }
    curptr = (struct dirlist *) malloc(strlen(cwd) + 1 + sizeof(struct dirlist));

    if (curptr != NULL) {
        curptr->next = head;
        head = curptr;
        curptr->dircode = dircode;
        strcpy(curptr->dirname, cwd);
    }
    return (0);
}

/*************************************************************************/
/* FUNCTION  : show_banner                                               */
/* PURPOSE   : Display a banner on the user's terminal before login      */
/* ARGUMENTS : reply code to use                                         */
/*************************************************************************/
 
void show_banner(int msgcode)
{
    char *crptr,
      linebuf[1024],
      outbuf[1024];
    struct aclmember *entry = NULL;
    FILE *infile;

#if defined(VIRTUAL) && defined(OLDVIRT)
    extern int virtual_mode;
    extern int virtual_ftpaccess;
    extern char virtual_banner[];

    if (virtual_mode && !virtual_ftpaccess) {
        infile = fopen(virtual_banner, "r");
	if (infile) {
 	    while (fgets(linebuf, 255, infile) != NULL) {
	           if ((crptr = strchr(linebuf, '\n')) != NULL)
		        *crptr = '\0';
		   msg_massage(linebuf, outbuf);
		   lreply(msgcode, "%s", outbuf);
		 }
	    fclose(infile);
#ifndef NO_SUCKING_NEWLINES
	    lreply(msgcode, "");
#endif
	  }
      }
    else {
#endif
      /* banner <path> */
      while (getaclentry("banner", &entry)) {
	    infile = fopen(ARG0, "r");
	    if (infile) {
	        while (fgets(linebuf, 255, infile) != NULL) {
		  if ((crptr = strchr(linebuf, '\n')) != NULL)
		    *crptr = '\0';
		  msg_massage(linebuf, outbuf);
		  lreply(msgcode, "%s", outbuf);
		}
		fclose(infile);
#ifndef NO_SUCKING_NEWLINES
		lreply(msgcode, "");
#endif
	    }
	 }
#if defined(VIRTUAL) && defined(OLDVIRT)
    }
#endif
  }
/*************************************************************************/
/* FUNCTION  : show_message                                              */
/* PURPOSE   : Display a message on the user's terminal if the current   */
/*             conditions are right                                      */
/* ARGUMENTS : reply code to use, LOG_IN|CMD                             */
/*************************************************************************/

void show_message(int msgcode, int mode)
{
    char *crptr,
      linebuf[1024],
      outbuf[1024],
      class[MAXPATHLEN],
      cwd[MAXPATHLEN];
    int show,
      which;
    struct aclmember *entry = NULL;
    FILE *infile;

    if (mode == C_WD && show_message_everytime == 0 && cwd_beenhere(1) != 0)
	return;

#ifdef HAVE_GETCWD
    getcwd(cwd,MAXPATHLEN-1);
#else
    getwd(cwd);
#endif
    acl_getclass(class);

    /* message <path> [<when> [<class>]] */
    while (getaclentry("message", &entry)) {
        if (!ARG0)
            continue;
        show = 0;

        if (mode == LOG_IN && (!ARG1 || !strcasecmp(ARG1, "login")))
            if (!ARG2)
                show++;
            else {
                for (which = 2; (which < MAXARGS) && ARG[which]; which++)
                    if (strcasecmp(class, ARG[which]) == 0)
                        show++;
            }
        if (mode == C_WD && ARG1 && !strncasecmp(ARG1, "cwd=", 4) &&
            (!strcmp((ARG1) + 4, cwd) || *(ARG1 + 4) == '*' ||
            !fnmatch((ARG1) + 4, cwd, FNM_PATHNAME)))
            if (!ARG2)
                show++;
            else {
                for (which = 2; (which < MAXARGS) && ARG[which]; which++)
                    if (strcasecmp(class, ARG[which]) == 0)
                        show++;
            }
        if (show && (int)strlen(ARG0) > 0) {
            infile = fopen(ARG0, "r");
            if (infile) {
                while (fgets(linebuf, 255, infile) != NULL) {
                    if ((crptr = strchr(linebuf, '\n')) != NULL)
                        *crptr = '\0';
                    msg_massage(linebuf, outbuf);
                    lreply(msgcode, "%s", outbuf);
                }
                fclose(infile);
#ifndef NO_SUCKING_NEWLINES
                lreply(msgcode, "");
#endif
            }
        }
    }
}

/*************************************************************************/
/* FUNCTION  : show_readme                                               */
/* PURPOSE   : Display a message about a README file to the user if the  */
/*             current conditions are right                              */
/* ARGUMENTS : pointer to ACL buffer, reply code, LOG_IN|C_WD            */
/*************************************************************************/

void show_readme(int code, int mode)
{
    char **filelist,
      **sfilelist,
      class[MAXPATHLEN],
      cwd[MAXPATHLEN];
    int show,
      which,
      days;
    time_t clock;

    struct stat buf;
    struct tm *tp;
    struct aclmember *entry = NULL;

#ifdef RATIO
    if( show_readme_everytime == 0 && cwd_beenhere(2) != 0)
	return;
#else /* RATIO */
    if (cwd_beenhere(2) != 0)
        return;
#endif /* RATIO */

#ifdef HAVE_GETCWD
    getcwd(cwd,MAXPATHLEN-1);
#else
    getwd(cwd);
#endif
    acl_getclass(class);

    /* readme  <path> {<when>} */
    while (getaclentry("readme", &entry)) {
        if (!ARG0)
            continue;
        show = 0;

        if (mode == LOG_IN && (!ARG1 || !strcasecmp(ARG1, "login")))
            if (!ARG2)
                show++;
            else {
                for (which = 2; (which < MAXARGS) && ARG[which]; which++)
                    if (strcasecmp(class, ARG[which]) == 0)
                        show++;
            }
        if (mode == C_WD && ARG1 && !strncasecmp(ARG1, "cwd=", 4)
            && (!strcmp((ARG1) + 4, cwd) || *(ARG1 + 4) == '*' ||
                !fnmatch((ARG1) + 4, cwd, FNM_PATHNAME)))
            if (!ARG2)
                show++;
            else {
                for (which = 2; (which < MAXARGS) && ARG[which]; which++)
                    if (strcasecmp(class, ARG[which]) == 0)
                        show++;
            }
        if (show) {
            globerr = NULL;
            filelist = ftpglob(ARG0);
            sfilelist = filelist;  /* save to free later */
            if (!globerr) {
                while (filelist && *filelist) {
                   errno = 0;
                   if (!stat(*filelist, &buf) &&
                       (buf.st_mode & S_IFMT) == S_IFREG) {
                       lreply(code, "Please read the file %s", *filelist);
                       time(&clock);
                       tp = localtime(&clock);
                       days = 365 * tp->tm_year + tp->tm_yday;
                       tp = localtime((time_t *)&buf.st_mtime);
                       days -= 365 * tp->tm_year + tp->tm_yday;
/*
                       if (days == 0) {
                         lreply(code, "  it was last modified on %.24s - Today",
                           ctime((time_t *)&buf.st_mtime));
                       } else {
*/
                         lreply(code, 
                           "  it was last modified on %.24s - %d day%s ago",
                           ctime((time_t *)&buf.st_mtime), days, days == 1 ? "" : "s");
/*
                       }
*/
                   }
                   filelist++;
                }
            }
            if (sfilelist) {
                blkfree(sfilelist);
                free((char *) sfilelist);
            }
        }
    }
}

/*************************************************************************/
/* FUNCTION  : deny_badxfertype                                          */
/* PURPOSE   : If user is in ASCII transfer mode and tries to retrieve a */
/*             binary file, abort transfer and display appropriate error */
/* ARGUMENTS : message code to use for denial, path of file to check for */
/*             binary contents or NULL to assume binary file             */
/*************************************************************************/

int deny_badasciixfer(int msgcode, char *filepath)
{

    if (type == TYPE_A && !*filepath) {
        reply(msgcode, "This is a BINARY file, using ASCII mode to transfer will corrupt it.");
        return (1);
    }
    /* The hooks are here to prevent transfers of actual binary files, not
     * just TAR or COMPRESS mode files... */
    return (0);
}

/*************************************************************************/
/* FUNCTION  : is_shutdown                                               */
/* PURPOSE   :                                                           */
/* ARGUMENTS :                                                           */
/*************************************************************************/

int is_shutdown(int quiet, int new)
{
    static struct tm tmbuf;
    static struct stat s_last;
    static time_t last = 0,
      shut,
      deny,
      disc;
    static int valid;
    static char text[2048];

    struct stat s_cur;

    FILE *fp;

    int deny_off,
      disc_off;

    time_t curtime = time(NULL);

    char buf[1024],
      linebuf[1024];

    if (Shutdown[0] == '\0' || stat(Shutdown, &s_cur))
        return (0);

    if (s_last.st_mtime != s_cur.st_mtime) {
        s_last = s_cur;
        valid = 0;

        fp = fopen(Shutdown, "r");
        if (fp == NULL)
            return (0);
        fgets(buf, sizeof(buf), fp);
        if (sscanf(buf, "%d %d %d %d %d %d %d", &tmbuf.tm_year, &tmbuf.tm_mon,
        &tmbuf.tm_mday, &tmbuf.tm_hour, &tmbuf.tm_min, &deny, &disc) != 7) {
            fclose(fp);
            return (0);
        }
        valid = 1;
        deny_off = 3600 * (deny / 100) + 60 * (deny % 100);
        disc_off = 3600 * (disc / 100) + 60 * (disc % 100);

        tmbuf.tm_year -= 1900;
        tmbuf.tm_isdst = -1;
        shut = mktime(&tmbuf);
        strcpy(shuttime, ctime(&shut));

        disc = shut - disc_off;
        strcpy(disctime, ctime(&disc));

        deny = shut - deny_off;
        strcpy(denytime, ctime(&deny));

        text[0] = '\0';

        while (fgets(buf, sizeof(buf), fp) != NULL) {
            msg_massage(buf, linebuf);
            if ((strlen(text) + strlen(linebuf)) < sizeof(text))
                strcat(text, linebuf);
        }

        fclose(fp);
    }
    if (!valid)
        return (0);

    /* if last == 0, then is_shutdown() only called with quiet == 1 so far */
    if (last == 0 && !quiet) {
        autospout = text;       /* warn them for the first time */
        autospout_free = 0;
        last = curtime;
    }
    /* if a new connection and past deny time, tell caller to drop 'em */
    if (new && curtime > deny)
        return (1);

    /* if past disconnect time, tell caller to drop 'em */
    if (curtime > disc)
        return (1);

    /* if less than 60 seconds to disconnection, warn 'em continuously */
    if (curtime > (disc - 60) && !quiet) {
        autospout = text;
        autospout_free = 0;
        last = curtime;
    }
    /* if less than 15 minutes to disconnection, warn 'em every 5 mins */
    if (curtime > (disc - 60 * 15)) {
        if ((curtime - last) > (60 * 5) && !quiet) {
            autospout = text;
            autospout_free = 0;
            last = curtime;
        }
    }
    /* if less than 24 hours to disconnection, warn 'em every 30 mins */
    if (curtime < (disc - 24 * 60 * 60) && !quiet) {
        if ((curtime - last) > (60 * 30)) {
            autospout = text;
            autospout_free = 0;
            last = curtime;
        }
    }
    /* if more than 24 hours to disconnection, warn 'em every 60 mins */
    if (curtime > (disc - 24 * 60 * 60) && !quiet) {
        if ((curtime - last) >= (24 * 60 * 60)) {
            autospout = text;
            autospout_free = 0;
            last = curtime;
        }
    }
    return (0);
}

void newer(char *date, char *path, int showlots)
{
    struct tm tm;

    if (sscanf(date, "%04d%02d%02d%02d%02d%02d",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {

        tm.tm_year -= 1900;
        tm.tm_mon--;
        tm.tm_isdst = -1;
        newer_time = mktime(&tm);
        dout = dataconn("file list", (off_t) -1, "w");

        if (dout != NULL) {
            /* As ftw allocates storage it needs a chance to cleanup, setting
             * ftwflag prevents myoob from calling longjmp, incrementing
             * ftwflag instead which causes check_newer to return non-zero
             * which makes ftw return. */
            ftwflag = 1;
            transflag++;
            show_fullinfo = showlots;
#if defined(HAVE_FTW_H)
            ftw(path, check_newer, -1);
#else
            treewalk(path, check_newer, -1, NULL);
#endif

            /* don't send a reply if myoob has already replied */
            if (ftwflag == 1) {
                if (ferror(dout) != 0)
                    perror_reply(550, "Data connection");
                else
                    reply(226, "Transfer complete.");
            }

            fclose(dout);
            data = -1;
            pdata = -1;
            transflag = 0;
            ftwflag = 0;
        }
    } else
        reply(501, "Bad DATE format");
}

int type_match(char *typelist)
{
    if (anonymous && strcasestr(typelist, "anonymous"))
        return (1);
    if (guest && strcasestr(typelist, "guest"))
        return (1);
    if (!guest && !anonymous && strcasestr(typelist, "real"))
        return (1);

    return (0);
}

int path_compare(char *p1, char *p2)
{
    if ( fnmatch(p1, p2, NULL) == 0 ) /* 0 means they matched */
        return(strlen(p1));
    else
        return(-2);
}

void expand_id()
{
    struct aclmember *entry = NULL;
    struct passwd *pwent;
    struct group *grent;
    char buf[BUFSIZ];

    while (getaclentry("upload", &entry) && ARG0 && ARG1 && ARG2 != NULL) {
      if ( (strcasecmp(ARG0, "absolute")==0)
         ||(strcasecmp(ARG0, "relative")==0)
         ||(strcasecmp(ARG0,"-")==0)) {
           ARG3=ARG4;
           ARG4=ARG5;
      }
      
      if (ARG3 && ARG4) {
          if ((ARG3[0] != '*') || (ARG3[1] != '\0')) {
              if (ARG3[0] != '%')
#ifdef OTHER_PASSWD
                  pwent = bero_getpwnam(ARG3, _path_passwd);
#else
                  pwent = getpwnam(ARG3);
#endif
              if (ARG3[0] == '%') sprintf(buf, "%s", ARG3+1); else
              if (pwent)  sprintf(buf, "%d", pwent->pw_uid);
              else        sprintf(buf, "%d", 0);
              ARG3 = (char *) malloc(strlen(buf) + 1);
              strcpy(ARG3, buf);
          }
          if ((ARG4[0] != '*') || (ARG4[1] != '\0')) {
              if (ARG4[0] != '%')
                  grent = getgrnam(ARG4);
              if (ARG4[0] == '%') sprintf(buf, "%s", ARG4+1); else
              if (grent)  sprintf(buf, "%d", grent->gr_gid);
              else        sprintf(buf, "%d", 0);
              ARG4 = (char *) malloc(strlen(buf) + 1);
              strcpy(ARG4, buf);
              endgrent();
          }
      }
    }
}

int fn_check(char *name)
{
  /* check to see if this is a valid file name... path-filter <type>
   * <message_file> <allowed_charset> <disallowed> */

  struct aclmember *entry = NULL;
  int   j;
  char *sp;
  char *path;
#ifdef M_UNIX
# ifdef HAVE_REGEX
  char *regp;
# endif
#endif

#ifdef HAVE_REGEXEC
  regex_t regexbuf;
  regmatch_t regmatchbuf;
#endif

  while (getaclentry("path-filter", &entry) && ARG0 != NULL) {
      if (type_match(ARG0) && ARG1 && ARG2) {

		  /*
		   * check *only* the basename
		   */

		  if (path = strrchr(name, '/'))  ++path;
		  else	path = name;

          /* is it in the allowed character set? */
#if defined(HAVE_REGEXEC)
          if (regcomp(&regexbuf, ARG2, REG_EXTENDED) != 0) {
              reply(550, "REGEX error");
#elif defined(HAVE_REGEX)
          if ((sp = regcmp(ARG2, (char *) 0)) == NULL) {
              reply(550, "REGEX error");
#else
          if ((sp = re_comp(ARG2)) != 0) {
              perror_reply(550, sp);
#endif
              return(0);
          }
#if defined(HAVE_REGEXEC)
          if (regexec(&regexbuf, path, 1, &regmatchbuf, 0) != 0) {
#elif defined(HAVE_REGEX)
# ifdef M_UNIX
          regp = regex(sp, path);
          free(sp);
          if (regp == NULL) {
# else
          if ((regex(sp, path)) == NULL) {
# endif
#else
          if ((re_exec(path)) != 1) {
#endif
              pr_mesg(550, ARG1);
              reply(550, "%s: Permission denied. (Filename (accept))", name);
              return(0);
          }
          /* is it in any of the disallowed regexps */

          for (j = 3; j < MAXARGS; ++j) {
              /* ARGj == entry->arg[j] */
              if (entry->arg[j]) {
#if defined(HAVE_REGEXEC)
                  if (regcomp(&regexbuf, entry->arg[j], REG_EXTENDED) != 0) {
                      reply(550, "REGEX error");
#elif defined(HAVE_REGEX)
                  if ((sp = regcmp(entry->arg[j], (char *) 0)) == NULL) {
                      reply(550, "REGEX error");
#else
                  if ((sp = re_comp(entry->arg[j])) != 0) {
                      perror_reply(550, sp);
#endif
                      return(0);
                  }
#if defined(HAVE_REGEXEC)
                  if (regexec(&regexbuf, path, 1, &regmatchbuf, 0) == 0) {
#elif defined(HAVE_REGEX)
# ifdef M_UNIX
                  regp = regex(sp, path);
                  free(sp);
                  if (regp != NULL) {
# else
                  if ((regex(sp, path)) != NULL) {
# endif
#else
                  if ((re_exec(path)) == 1) {
#endif
                      pr_mesg(550, ARG1);
                      reply(550, "%s: Permission denied. (Filename (deny))", name);
                      return(0);
                  }
              }
          }
      }
  }
  return(1);
}

int dir_check(char *name, uid_t *uid, gid_t *gid, int *d_mode, int *valid)
{
  struct aclmember *entry = NULL;

  int i,
    match_value = -1;
  char *ap2 = NULL,
       *ap3 = NULL,
       *ap4 = NULL,
       *ap5 = NULL,
       *ap6 = NULL,
       *ap7 = NULL;
  char cwdir[BUFSIZ];
  char *pwdir;
  char abspwdir[BUFSIZ];
  char relpwdir[BUFSIZ];
  char path[BUFSIZ];
  char *sp;
  struct stat stbuf;
  int stat_result;
  extern struct passwd *pw;
  extern char *home;

  *valid = 0;
  /* what's our current directory? */

  strcpy(path, name);
  if (sp = strrchr(path, '/'))  *sp = '\0';
  else strcpy(path, ".");

  if ((fb_realpath(path, cwdir)) == NULL) {
    perror_reply(550, "Could not determine cwdir");
    return(-1);
  }

  if ((fb_realpath(home, relpwdir)) == NULL) {
    perror_reply(550, "Could not determine pwdir");
    return(-1);
  }
  
  if ((wu_realpath(home, abspwdir, chroot_path)) == NULL) {
    perror_reply(550, "Could not determine pwdir");
    return(-1);
  }

  while (getaclentry("upload", &entry) && ARG0 && ARG1 && ARG2 != NULL) {
      if (strcasecmp(ARG0, "relative")==0)
        pwdir=relpwdir;
      else
        pwdir=abspwdir;
      if ((strcasecmp(ARG0, "absolute")==0)
        ||(strcasecmp(ARG0, "relative")==0)
        ||(strcasecmp(ARG0, "-")==0)) {
        ARG0=ARG1;
        ARG1=ARG2;
        ARG2=ARG3;
        ARG3=ARG4;
        ARG4=ARG5;
        ARG5=ARG6;
        ARG6=ARG7;
        ARG7=ARG8;
      }
      if (  (   (0 == strcmp       (ARG0, "*"  ))
             || (0 == strcmp       (ARG0, pwdir))
             || (0 <  path_compare (ARG0, pwdir)))
           && ( ( i = path_compare (ARG1, cwdir) ) >= match_value )
          ) {
          match_value = i;
          ap2 = ARG2;
          if (ARG3)  ap3 = ARG3;
          else       ap3 = NULL;
          if (ARG4)  ap4 = ARG4;
          else       ap4 = NULL;
          if (ARG5)  ap5 = ARG5;
          else       ap5 = NULL;
          if (ARG6)  ap6 = ARG6;
          else       ap6 = NULL;
          if (ARG7)  ap7 = ARG7;
          else       ap7 = NULL;
      }
  }
  if (anonymous && (match_value < 0)) {
      reply(550, "%s: Permission denied. (Upload dirs)", name);
      return(0);
  }
  if ( (ap2 && !strcasecmp(ap2, "no")) ||
       (ap3 && !strcasecmp(ap3, "nodirs")) ||
       (ap6 && !strcasecmp(ap6, "nodirs")) ) {
      reply(550, "%s: Permission denied. (Upload dirs)", name);
      return(0);
  }
  if ((ap3 && *ap3 == '*') || (ap4 && *ap4 == '*'))
    stat_result = stat(path, &stbuf);
  if (ap3)
    if ((ap3[0] != '*') || (ap3[1] != '\0'))
     *uid = atoi(ap3);    /* the uid  */
    else
     if (stat_result == 0)
       *uid = stbuf.st_uid;
  if (ap4)
    if ((ap4[0] != '*') || (ap4[1] != '\0'))
     *gid = atoi(ap4);    /* the gid */
    else
     if (stat_result == 0)
       *gid = stbuf.st_gid;
  if (ap7) {
     sscanf(ap7, "%o", d_mode);
     *valid = 1;
   } else if (ap5) {
     sscanf(ap5, "%o", d_mode);
     if (*d_mode & 0600)
        *d_mode |= 0100;
     if (*d_mode & 0060)
        *d_mode |= 0010;
     if (*d_mode & 0006)
        *d_mode |= 0001;
     *valid = 1;
   }
  return(1);
}

int upl_check(char *name, uid_t *uid, gid_t *gid, int *f_mode, int *valid)
{
  int  match_value = -1;
  char cwdir[BUFSIZ];
  char *pwdir;
  char abspwdir[BUFSIZ];
  char relpwdir[BUFSIZ];
  char path[BUFSIZ];
  char *sp;
  int  i;
  struct stat stbuf;
  int stat_result;

  char *ap1 = NULL,
   *ap2 = NULL,
   *ap3 = NULL,
   *ap4 = NULL,
   *ap5 = NULL;

  struct aclmember *entry = NULL;
  extern struct passwd *pw;
  extern char *home;

  *valid = 0;

      /* what's our current directory? */

      strcpy(path, name);
      if (sp = strrchr(path, '/'))  *sp = '\0';
      else strcpy(path, ".");

      if ((fb_realpath(path, cwdir)) == NULL) {
          perror_reply(550, "Could not determine cwdir");
          return(-1);
      }
     
     if((wu_realpath(home, abspwdir, chroot_path)) == NULL) {
          perror_reply(553, "Could not determine pwdir");
          return(-1);
      }
      
      if((fb_realpath(home, relpwdir)) == NULL) {
          perror_reply(553, "Could not determine pwdir");
          return(-1);
      }

      /* we are doing a "best match"... ..so we keep track of what "match
       * value" we have received so far... */

      while (getaclentry("upload", &entry) && ARG0 && ARG1 && ARG2 != NULL) {
if (strcasecmp (ARG0, "relative") == 0)
  pwdir = relpwdir;
else
  pwdir = abspwdir;
if ((strcasecmp (ARG0, "absolute") == 0)
||  (strcasecmp (ARG0, "relative") == 0)
||  (strcmp (ARG0, "-") == 0)) {
  ARG0 = ARG1;
  ARG1 = ARG2;
  ARG2 = ARG3;
  ARG3 = ARG4;
  ARG4 = ARG5;
  ARG5 = ARG6;
  ARG6 = ARG7;
  ARG7 = ARG8;
}
      if (   (   (0 == strcmp       (ARG0, "*"  ))
              || (0 == strcmp       (ARG0, pwdir))
              || (0 <  path_compare (ARG0, pwdir)))
          && ( ( i = path_compare (ARG1, cwdir) ) >= match_value )
         ) {
              match_value = i;
              ap1 = ARG1;
              ap2 = ARG2;
              if (ARG3) ap3 = ARG3;
              else      ap3 = NULL;
              if (ARG4) ap4 = ARG4;
              else      ap4 = NULL;
              if (ARG5) ap5 = ARG5;
              else      ap5 = NULL;
          }
      }

      if (ap3 && ( (!strcasecmp("dirs",ap3)) || (!strcasecmp("nodirs", ap3)) ))
        ap3 = NULL;

      /* if we did get matches... ..else don't do any of this stuff */
      if (match_value >= 0) {
          if (!strcasecmp(ap2, "yes")) {
              if ((ap3 && *ap3 == '*') || (ap4 && *ap4 == '*'))
                  stat_result = stat(path, &stbuf);
              if (ap3)
                if ((ap3[0] != '*') || (ap3[1] != '\0'))
                  *uid = atoi(ap3);    /* the uid  */
                else
                 if (stat_result == 0)
                   *uid = stbuf.st_uid;
              if (ap4) {
                if ((ap4[0] != '*') || (ap4[1] != '\0'))
                  *gid = atoi(ap4);    /* the gid  */
                else
                 if (stat_result == 0)
                   *gid = stbuf.st_gid;
		  *valid = 1;
		}
              if (ap5)
                  sscanf(ap5, "%o", f_mode); /* the mode */
          } else {
              reply(553, "%s: Permission denied. (Upload)", name);
              return(-1);
          }
      } else {
          /*
           * upload defaults to "permitted"
           */
          /* Not if anonymous */ if (anonymous) {
              reply(553, "%s: Permission denied. (Upload)", name);
              return (-1);
          }
          return(1);
      }

  return(match_value);
}

int
#ifdef __STDC_
del_check(char *name)
#else
del_check(name)
char *name;
#endif
{
  int pdelete = (anonymous? 0 : 1);
  struct aclmember *entry = NULL;

  while (getaclentry("delete", &entry) && ARG0 && ARG1 != NULL) {
      if (type_match(ARG1))
          if (anonymous) {
              if (*ARG0 == 'y')
                  pdelete = 1;
          } else
          if (*ARG0 == 'n')
              pdelete = 0;
  }
  
  if (!pdelete) {
      reply(553, "%s: Permission denied. (Delete)", name);
      return(0);
  } else {
      return(1);
  }
}

int regexmatch(char *name, char *rgexp)
{
  
#ifdef M_UNIX
# ifdef HAVE_REGEX
  char *regp;
# endif
#endif
  
#ifdef HAVE_REGEXEC
  regex_t regexbuf;
  regmatch_t regmatchbuf;
#else
  char *sp;
#endif

#if defined(HAVE_REGEXEC)
      if (regcomp(&regexbuf, rgexp, REG_EXTENDED) != 0) {
          reply(553, "REGEX error");
#elif defined(HAVE_REGEX)
      if ((sp = regcmp(rgexp, (char *) 0)) == NULL) {
          reply(553, "REGEX error");
#else
      if ((sp = re_comp(rgexp)) != 0) {
          perror_reply(553, sp);
#endif
          return(0);
      }
  
#if defined(HAVE_REGEXEC)
      if (regexec(&regexbuf, name, 1, &regmatchbuf, 0) != 0) {
#elif defined(HAVE_REGEX)
# ifdef M_UNIX
      regp = regex(sp, name);
      free(sp);
      if (regp == NULL) {
# else
      if ((regex(sp, name)) == NULL) {
# endif
#else
      if ((re_exec(name)) != 1) {
#endif
              return(0);
      }
      return(1);
}

#define lbasename(x) (strrchr(x,'/')?1+strrchr(x,'/'):x)

static int allow_retrieve (char *name)
{
  char realname[MAXPATHLEN+1];
  char localname[MAXPATHLEN+1];
  char *whichname;
  int i;
  int len;
  struct aclmember *entry = NULL;
  char *p, *q;

  if (name == (char *)NULL || *name == '\0')
    return 0;

  wu_realpath (name, localname, NULL);
  wu_realpath (name, realname, chroot_path);

   while (getaclentry("allow-retrieve", &entry)) {
        if (ARG0 == (char *)NULL)
            continue;
        whichname = realname;
        i = 0;
        if (entry->arg[0])
          if (strcasecmp (entry->arg[0], "absolute") == 0)
            i++;
          else if (strcasecmp (entry->arg[0], "relative") == 0) {
            i++;
            whichname = localname;
          } else if (strcmp (entry->arg[0], "-") == 0)
            i++;
        for ( ; i< MAXARGS &&
             ((q = entry->arg[i]) != (char *)NULL) && (q[0] !='\0'); i++)
        {
          len = strlen(q);
          p = (q[0] == '/') ? whichname : lbasename (whichname);
          if ( ((q[0] == '/') && (q[len-1] == '/') && (strncmp(q, p, len) == 0))
          ||   (strcmp(p, q) == 0)
          ||   !fnmatch(p,q))
            return 1;        }
      }
   return 0;
}

int checknoretrieve (char *name)
{
  char realname[MAXPATHLEN+1];
  char localname[MAXPATHLEN+1];
  char *whichname;
  int i;
  int len;
  struct aclmember *entry = NULL;
  char *p, *q;

  if (name == (char *)NULL || *name == '\0')
    return 0; 

   wu_realpath (name, localname, NULL);
   wu_realpath (name, realname, chroot_path);

   while (getaclentry("noretrieve", &entry)) {
        if (ARG0 == (char *)NULL)
            continue;
        whichname=realname;
        i=0;
        if (entry->arg[0])
          if (strcasecmp (entry->arg[0], "absolute") == 0)
            i++;
          else if (strcasecmp (entry->arg[0], "relative") == 0) {
            i++;
            whichname=localname;
          } else if (strcasecmp (entry->arg[0], "-") == 0)
            i++;
            
	for ( ; i< MAXARGS && 
	     ((q = entry->arg[i]) != (char *)NULL) && (q[0] != '\0'); i++)
        {
          len = strlen(q);
          p = (q[0] == '/') ? whichname : lbasename(whichname);
          if ( ((q[0] == '/') && (q[len-1] == '/') && (strncmp(q, p, len) == 0))
            || (strcmp(p, q) == 0)
            || !fnmatch(p,q)) {
            if (!allow_retrieve(name)) {
	      reply (550, "%s is marked unretrievable", localname);
	      return 1;
	    }
	  }
        }
      }
   return 0;
}

#ifdef QUOTA

#ifdef QUOTA_DEVICE 

#ifndef MNTMAXSTR
#define MNTMAXSTR 2048 /* And hope it's enough */
#endif

path_to_device (char *pathname,char *result)
{
  FILE *fp;
  struct mntent *mp;
  struct mount_ent
    {
      char mnt_fsname[MNTMAXSTR], mnt_dir[MNTMAXSTR];
      struct mount_ent *next;
    }
  mountent;
  struct mount_ent *current, *start, *new;
  char path[1024], mnt_dir[1024], *pos;
  int flag = 1;

  start = current = NULL;
  fp = setmntent (_PATH_MNTTAB, "r");

  while (mp = getmntent (fp))
    {
      if (!(new = (struct mount_ent *) malloc (sizeof (mountent))))
       {
         perror ("malloc");
         flag = 0;
       }

      if (!start)
       start = current = new;
      else
       current = current->next = new;

      strncpy (current->mnt_fsname, mp->mnt_fsname,
              strlen (mp->mnt_fsname) + 1);
      strncpy (current->mnt_dir, mp->mnt_dir, strlen (mp->mnt_dir) + 1);
    }
  endmntent (fp);
  current->next = NULL;

  wu_realpath (pathname, path, chroot_path);

  while (*path && flag)
    {
      current = start;
      while (current && flag)
       {
         if (strcmp (current->mnt_dir, "swap"))
           {
             wu_realpath (current->mnt_dir, mnt_dir, chroot_path);
             if (!strcmp (mnt_dir, path))
               {
                 flag = 0;
                 /* no support for remote quota yet */
                 if (!index (current->mnt_fsname, ':'))
                   strcpy (result, current->mnt_fsname);
               }
           }
         current = current->next;
       }
      if (!((pos = strrchr (path, '/')) - path) && strlen (path) > 1)
       strcpy (path, "/");
      else
/*       path[pos - path] = '\0';  */
        *pos='\0';
    }
    while (current)
    {
      new = current->next;
      free (current);
      current = new;
    }
}
#endif

void get_quota(char *fs,int uid)
{
#ifdef QUOTA_DEVICE
       char mnt_fsname[MNTMAXSTR];

       path_to_device (fs, mnt_fsname);
       quotactl (Q_GETQUOTA, mnt_fsname, uid, (char *) &quota);
#else
  quotactl(fs,QCMD(Q_GETQUOTA,USRQUOTA),uid,&quota);
#endif
}
 
char *time_quota(long curstate, long softlimit, long timelimit, char *timeleft)
{
       struct timeval tv;

       gettimeofday(&tv, NULL);
       if (softlimit && curstate >= softlimit) {
               if (timelimit == 0) {
                       strcpy(timeleft, "NOT STARTED");
               } else if (timelimit > tv.tv_sec) {
                       fmttime(timeleft, timelimit - tv.tv_sec);
               } else {
                       strcpy(timeleft, "EXPIRED");
               }
       } else {
               *timeleft = '\0';
       }
       return(timeleft);
}

int fmttime(char *buf, register long time)
{
       int i;
       static struct {
               int c_secs;             /* conversion units in secs */
               char * c_str;           /* unit string */
       } cunits [] = {
               {60*60*24*28, "months"},
               {60*60*24*7, "weeks"},
               {60*60*24, "days"},
               {60*60, "hours"},
               {60, "mins"},
               {1, "secs"}
       };

       if (time <= 0) {
               strcpy(buf, "EXPIRED");
               return;
       }
       for (i = 0; i < sizeof(cunits)/sizeof(cunits[0]); i++) {
               if (time >= cunits[i].c_secs)
                       break;
       }
       sprintf(buf, "%.1f %s", (double)time/cunits[i].c_secs, cunits[i].c_str);
}
#endif

#ifdef QUOTA
#if !defined(HAVE_SYS_QUOTA_H) && defined(HAVE_LINUX_QUOTA_H) /* Linux libc 5 */
/* I have no idea why I can't find 'quotactl()' in my libs, here's the source - GAL */

/*
 * QUOTA    An implementation of the diskquota system for the LINUX
 *          operating system. QUOTA is implemented using the BSD systemcall
 *          interface as the means of communication with the user level.
 *          Should work for all filesystems because of integration into the
 *          VFS layer of the operating system.
 *          This is based on the Melbourne quota system wich uses both user and
 *          group quota files.
 *
 *          System call interface.
 *
 * Version: $Id: quotactl.c,v 2.3 1995/07/23 09:58:06 mvw Exp mvw $
 *
 * Author:  Marco van Wieringen <mvw@planets.ow.nl> <mvw@tnix.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License
 *          as published by the Free Software Foundation; either version
 *          2 of the License, or (at your option) any later version.
 */
#if defined(__alpha__)
#include <errno.h>
#include <sys/types.h>
#include <syscall.h>
#include <asm/unistd.h>

int quotactl(int cmd, const char * special, int id, caddr_t addr)
{
	return syscall(__NR_quotactl, cmd, special, id, addr);
}
#else
#include <sys/types.h>
#define __LIBRARY__
#include <linux/unistd.h>

_syscall4(int, quotactl, int, cmd, const char *, special, int, id, caddr_t, addr);
#endif
#endif
#endif

#ifdef THROUGHPUT

int file_compare(char *patterns, char *file)
{
    char buf[BUFSIZ];
    char *cp;
    char *cp2;
    int i;
    int matches = 0;

    strcpy(buf, patterns);
    i = strlen(buf);
    buf[i++] = ',';
    buf[i++] = '\0';

    cp = buf;
    while ((cp2 = strchr(cp, ',')) != NULL) {
        *cp2++ = '\0';
        if (fnmatch(cp, file, 0) == 0) {
            matches = 1;
            break;
        }
    }
    return matches;
}

int remote_compare(char *patterns)
{
    char buf[BUFSIZ];
    char *cp;
    char *cp2;
    int i;
    int matches = 0;

    strcpy(buf, patterns);
    i = strlen(buf);
    buf[i++] = ',';
    buf[i++] = '\0';

    cp = buf;
    while ((cp2 = strchr(cp, ',')) != NULL) {
        *cp2++ = '\0';
        if (hostmatch (cp, remoteaddr, remotehost)) {
            matches = 1;
            break;
        }
    }
    return matches;
}

void throughput_calc(char *name, int *bps, double *bpsmult)
{
    int match_value = -1;
    char cwdir[BUFSIZ];
    char pwdir[BUFSIZ];
    char path[BUFSIZ];
    char file[BUFSIZ];
    char x[BUFSIZ];
    char *ap1 = NULL, *ap2 = NULL, *ap3 = NULL, *ap4 = NULL;
    struct aclmember *entry = NULL;
    extern struct passwd *pw;
    extern char *home;
    char *sp;
    int i;

    /* default is maximum throughput */
    *bps = -1;
    *bpsmult = 1.0;

    /* what's our current directory? */
    strcpy(path, name);
    if (sp = strrchr(path, '/'))
        *sp = '\0';
    else
        strcpy(path, ".");
    if (sp = strrchr(name, '/'))
        strcpy(file, sp + 1);
    else
        strcpy(file, name);
    if ((fb_realpath(path, cwdir)) == NULL) {
        perror_reply(553, "Could not determine cwdir");
        return;
    }

    wu_realpath(home, pwdir, chroot_path);

    /* find best matching entry */
    while (getaclentry("throughput", &entry) && ARG0 && ARG1 && ARG2 && ARG3 && ARG4 && ARG5 != NULL) {
        if ((!strcmp(ARG0, pwdir)) &&
            ((i = path_compare(ARG1, cwdir)) >= match_value)) {
            if (file_compare(ARG2, file)) {
                if (remote_compare(ARG5)) {
                    match_value = i;
                    ap3 = ARG3;
                    ap4 = ARG4;
                }
            }
        }
    }

    /* if we did get matches */
    if (match_value >= 0) {
        if (strcasecmp(ap3, "oo") == 0)
            *bps = -1;
        else
            *bps = atoi(ap3);
        if (strcmp(ap4, "-") == 0)
            *bpsmult = 1.0;
        else
            *bpsmult = atof(ap4);
    }
    return;
}

void throughput_adjust(char *name)
{
    int match_value = -1;
    char pwdir[BUFSIZ];
    char cwdir[BUFSIZ];
    char path[BUFSIZ];
    char file[BUFSIZ];
    char buf[BUFSIZ];
    char *ap1 = NULL, *ap2 = NULL, *ap3 = NULL, *ap4 = NULL, *ap5 = NULL;
    char **pap3;
    struct aclmember *entry = NULL;
    extern struct passwd *pw;
    extern char *home;
    char *sp;
    int i;

    /* what's our current directory? */
    strcpy(path, name);
    if (sp = strrchr(path, '/'))
        *sp = '\0';
    else
        strcpy(path, ".");
    if (sp = strrchr(name, '/'))
        strcpy(file, sp + 1);
    else
        strcpy(file, name);
    if ((fb_realpath(path, cwdir)) == NULL) {
        perror_reply(553, "Could not determine cwdir");
        return;
    }

    wu_realpath(home, pwdir, chroot_path);

    /* find best matching entry */
    while (getaclentry("throughput", &entry) && ARG0 && ARG1 && ARG2 && ARG3 && ARG4 && ARG5 != NULL) {
      if (   (   (0 == strcmp       (ARG0, "*"  ))
              || (0 == strcmp       (ARG0, pwdir))
              || (0 <  path_compare (ARG0, pwdir)))
          && ( ( i = path_compare (ARG1, cwdir) ) >= match_value )
         ) {
            if (file_compare(ARG2, file)) {
                if (remote_compare(ARG5)) {
                    match_value = i;
                    ap3 = ARG3;
                    pap3 = &ARG3;
                    ap4 = ARG4;
                }
            }
        }
    }

    /* if we did get matches */
    if (match_value >= 0) {
        if (strcasecmp(ap3, "oo") != 0) {
            if (strcmp(ap4, "-") != 0) {
                sprintf(buf, "%.0f", atoi(ap3) * atof(ap4));
                *pap3 = (char *) malloc(strlen(buf) + 1);
                strcpy(*pap3, buf);
            }
        }
    }
    return;
}

#endif

static int CheckMethod = 0;

void SetCheckMethod (const char *method)
{
    if ((strcasecmp (method, "md5"    ) == 0)
    ||  (strcasecmp (method, "rfc1321") == 0))
        CheckMethod = 0;
    else if ((strcasecmp (method, "crc"  ) == 0)
    ||       (strcasecmp (method, "posix") == 0))
        CheckMethod = 1;
    else {
        reply (500, "Unrecognized checksum method");
        return;
    }
    switch (CheckMethod) {
    default: reply (200, "Checksum method is now: MD5 (RFC1321)"); break;
    case 1:  reply (200, "Checksum method is now: CRC (POSIX)");   break;
    }
}

void ShowCheckMethod ()
{
    switch (CheckMethod) {
    default: reply (200, "Current checksum method: MD5 (RFC1321)"); break;
    case 1:  reply (200, "Current checksum method: CRC (POSIX)");   break;
    }
}

void CheckSum (const char *pathname)
{
    char *cmd;
    char buf [MAXPATHLEN];
    FILE* cmdf;
    FILE* ftpd_popen();
    struct stat st;

    if (stat (pathname, &st) == 0) {
        if ((st.st_mode & S_IFMT) != S_IFREG) {
            reply (500, "%s: not a plain file.", pathname);
            return;
        }
    } else {
        perror_reply (550, pathname);
        return;
    }

    switch (CheckMethod) {
    default: cmd = "/bin/md5sum"; break;
    case 1:  cmd = "/bin/cksum";  break;
    }

    if (strlen(cmd) + 1 + strlen (pathname) + 1 > sizeof(buf)) {
        reply (500, "Pathname too long");
        return;
    }
    sprintf(buf, "%s %s", cmd, pathname);

    cmdf = ftpd_popen(buf, "r", 0);
    if (!cmdf) {
        perror_reply(550, cmd);
    } else {
        if (fgets(buf, sizeof buf, cmdf)) {
            char *crptr = strchr (buf, '\n');
            if (crptr != NULL)
                *crptr = '\0';
            reply(200, "%s", buf);
        }
        ftpd_pclose(cmdf);
    }
}

void CheckSumLastFile ()
{
    extern char LastFileTransferred [];

    if (LastFileTransferred [0] == '\0')
        reply (500, "Nothing transferred yet");
    else
        CheckSum (LastFileTransferred);
}
