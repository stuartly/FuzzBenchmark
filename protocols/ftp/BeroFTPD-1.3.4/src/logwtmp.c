/* Copyright (c) 1988 The Regents of the University of California. All rights
 * reserved.
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
 * University of California, Berkeley and its contributors. 4. Neither the
 * name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * $Id: logwtmp.c,v 1.1.1.1 1998/08/21 18:10:33 root Exp $ from logwtmp.c 5.7 (Berkeley) 2/25/91 */

#include "config.h"

#include <sys/types.h>
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
#include <sys/stat.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <utmp.h>
#ifdef HAVE_UTMPX_H
#include <utmpx.h>
#ifdef HAVE_SAC_H
#include <sac.h>
#endif
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#else
#include <string.h>
#endif
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif
#ifdef __FreeBSD__
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "pathnames.h"

static int fd = -1;
#ifdef HAVE_UTMPX_H
static int fdx = -1;
#endif

/* Modified version of logwtmp that holds wtmp file open after first call,
 * for use with ftp (which may chroot after login, but before logout). */

void wu_logwtmp(char *line, char *name, char *host, int login)
{
    struct stat buf;
    struct utmp ut;

#ifdef HAVE_UTMPX_H
    struct utmpx utx;

    if (fdx < 0 && (fdx = open(WTMPX_FILE, O_WRONLY | O_APPEND, 0)) < 0) {
        syslog(LOG_ERR, "wtmpx %s %m", WTMPX_FILE);
        return;
    }

    if (fstat(fdx, &buf) == 0) {
        memset((void *)&utx, '\0', sizeof(utx));
	strncpy(utx.ut_user, name, sizeof(utx.ut_user));
	strncpy(utx.ut_host, host, sizeof(utx.ut_host));
	strncpy(utx.ut_id, "ftp", sizeof(utx.ut_id));
        strncpy(utx.ut_line, line, sizeof(utx.ut_line));
#ifdef HAVE_UTX_UT_SYSLEN
        utx.ut_syslen = strlen(utx.ut_host)+1;
#endif
        utx.ut_pid = getpid();
        time (&utx.ut_tv.tv_sec);
        if (login /* name && *name */){
            utx.ut_type = USER_PROCESS;
	}else{
            utx.ut_type = DEAD_PROCESS;
	}
        utx.ut_exit.e_termination = 0;
        utx.ut_exit.e_exit = 0;
        if (write(fdx, (char *) &utx, sizeof(struct utmpx)) !=
            sizeof(struct utmpx))
	  ftruncate(fdx, buf.st_size);
      }
#endif

#ifdef __FreeBSD__
      if (strlen(host) > UT_HOSTSIZE) {
	    struct hostent *hp = gethostbyname(host);

	    if (hp != NULL) {
		    struct in_addr in;

		    memmove(&in, hp->h_addr, sizeof(in));
		    host = inet_ntoa(in);
	    } else
		    host = "invalid hostname";
      }
#endif

    if (fd < 0 && (fd = open(_PATH_WTMP, O_WRONLY | O_APPEND, 0)) < 0) {
        syslog(LOG_ERR, "wtmp %s %m", _PATH_WTMP);
        return;
    }
    if (fstat(fd, &buf) == 0) {
        memset((void *)&ut, 0, sizeof(ut));
#ifdef HAVE_UT_UT_ID
        strncpy(ut.ut_id, "ftp", sizeof(ut.ut_id));
#endif
        strncpy(ut.ut_line, line, sizeof(ut.ut_line));
#ifdef HAVE_UT_UT_PID
        ut.ut_pid = getpid();
#endif
        if (login /* name && *name */){
#ifdef HAVE_UT_UT_NAME
	  strncpy(ut.ut_name, name, sizeof(ut.ut_name));
#else
	  strncpy(ut.ut_user, name, sizeof(ut.ut_user));
#endif
#ifdef HAVE_UT_UT_TYPE
	  ut.ut_type = USER_PROCESS;
#endif
	}
#ifdef HAVE_UT_UT_TYPE
        else
            ut.ut_type = DEAD_PROCESS;
#endif
#ifdef HAVE_UT_UT_EXIT_E_TERMINATION
        ut.ut_exit.e_termination = 0;
        ut.ut_exit.e_exit = 0;
#endif
#ifdef HAVE_UT_UT_HOST  /* does have host in utmp */
	if (login){
	  strncpy(ut.ut_host, host, sizeof(ut.ut_host));
	}else{
	  strncpy(ut.ut_host, "", sizeof(ut.ut_host));
	}
#endif
        time(&ut.ut_time);
        if (write(fd, (char *) &ut, sizeof(struct utmp)) !=
            sizeof(struct utmp))
              ftruncate(fd, buf.st_size);
    }
}
