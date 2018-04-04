/* Copyright (c) 1988 The Regents of the University of California. All rights
 * reserved.
 *
 * This code is derived from software written by Ken Arnold and published in
 * UNIX Review, Vol. 6, No. 8.
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
 */

#ifndef lint
static char rcsid[] = "@(#)$Id: popen.c,v 1.1.1.1 1998/08/21 18:10:33 root Exp $";
#endif /* not lint */

#include "config.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#ifdef HAVE_GETRLIMIT
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
#include <sys/resource.h>
#endif

#ifndef RLIM_INFINITY /* HPUX sucks... */
#define RLIM_INFINITY 0x7fffffff
#endif

#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL  "/dev/null"
#endif
/* 
 * Special version of popen which avoids call to shell.  This insures noone
 * may create a pipe to a hidden program as a side effect of a list or dir
 * command. 
 */
static int *pids;
static int fds;
#define MAX_ARGV 100
#define MAX_GARGV 1000
FILE * ftpd_popen(char *program, char *type, int closestderr)
{
    register char *cp;
    FILE *iop;
    int argc,
      gargc,
      pdes[2],
      pid,i,devnullfd;
    char **pop,
     *argv[MAX_ARGV],
     *gargv[MAX_GARGV],
     *vv[2];
    extern char **ftpglob(register char *v),
    **copyblk(register char **v),
     *strspl(register char *cp, register char *dp),
      *globerr;
#ifdef HAVE_GETRLIMIT
        struct rlimit rlp;

		rlp.rlim_cur = rlp.rlim_max = RLIM_INFINITY;
		if (getrlimit( RLIMIT_NOFILE, &rlp ) )
			return(NULL);
		fds = rlp.rlim_cur;
#else
#ifdef HAVE_GETDTABLESIZE
        if ((fds = getdtablesize()) <= 0)
            return (NULL);
#else
#ifdef HAVE_SYSCONF
       fds = sysconf(_SC_OPEN_MAX);
#else
#ifdef OPEN_MAX
    fds=OPEN_MAX; /* need to include limits.h somehow */
#else
    fds = 31; /* XXX -- magic cookie*/
#endif
#endif
#endif
#endif
    if (*type != 'r' && *type != 'w' || type[1])
        return (NULL);

    if (!pids) {
        if ((pids = (int *) malloc((u_int) (fds * sizeof(int)))) == NULL)
              return (NULL);
        memset((void *)pids, 0, fds * sizeof(int));
    }
    if (pipe(pdes) < 0)
        return (NULL);

    /* empty the array */
    memset(argv, 0, sizeof(argv));
    
    /* break up string into pieces */
    for (argc = 0, cp = program;argc < MAX_ARGV - 1 ; cp = NULL)
        if (!(argv[argc++] = strtok(cp, " \t\n")))
            break;

    /* glob each piece */
    gargv[0] = argv[0];
    for (gargc = argc = 1; argc < MAX_ARGV && argv[argc]; argc++) {
        if (!(pop = ftpglob(argv[argc])) || globerr != NULL) 
	{ /* globbing failed */
            vv[0] = strspl(argv[argc], "");
            vv[1] = NULL;
            pop = copyblk(vv);
        }
        argv[argc] = (char *) pop;  /* save to free later */
        while (*pop && gargc < (MAX_GARGV - 1 ))
            gargv[gargc++] = *pop++;
    }
    gargv[gargc] = NULL;

#ifdef SIGCHLD
    signal(SIGCHLD, SIG_DFL);
#endif
    iop = NULL;
    switch (pid = vfork()) {
    case -1:                    /* error */
        close(pdes[0]);
        close(pdes[1]);
        goto pfree;
        /* NOTREACHED */
    case 0:                 /* child */
        if (*type == 'r') {
            if (pdes[1] != 1) {
                dup2(pdes[1], 1);
                if (closestderr) {
                    close(2);
                    /* stderr output is written to fd 2, so make sure it isn't
                     * available to be assigned to another file */
                    if ((devnullfd = open(_PATH_DEVNULL, O_RDWR)) != -1) {
                        if (devnullfd != 2) {
                            dup2(devnullfd, 2);
                            close(devnullfd);
                        }
                    }
                }
                else 
                    dup2(pdes[1], 2);  /* stderr, too! */
                close(pdes[1]);
            }
            close(pdes[0]);
        } else {
            if (pdes[0] != 0) {
                dup2(pdes[0], 0);
                close(pdes[0]);
            }
            close(pdes[1]);
        }
	for (i = 3; i < fds; i++)
                close(i);
	/* begin CERT suggested fixes */
	close(0); 
        i = geteuid();
	delay_signaling(); /* we can't allow any signals while euid==0: kinch */
        seteuid(0);
        setgid(getegid());
        setuid(i);
	enable_signaling(); /* we can allow signals once again: kinch */
	/* end CERT suggested fixes */
	execv(gargv[0], gargv);
        _exit(1);
    }
    /* parent; assume fdopen can't fail...  */
    if (*type == 'r') {
        iop = fdopen(pdes[0], type);
        close(pdes[1]);
    } else {
        iop = fdopen(pdes[1], type);
        close(pdes[0]);
    }
    pids[fileno(iop)] = pid;

  pfree:for (argc = 1; argc < MAX_ARGV && argv[argc]; argc++) {
        blkfree((char **) argv[argc]);
        free((char *) argv[argc]);
    }
    return (iop);
}

ftpd_pclose(FILE * iop)
{
    register int fdes;
    int pid;
#if defined (HAVE_SIGEMPTYSET)
    sigset_t sig, omask;
    int stat_loc;
#else
    int omask;
#ifdef NeXT
    union wait stat_loc;
#else
    int stat_loc;
#endif
#endif


    /* pclose returns -1 if stream is not associated with a `popened'
     * command, or, if already `pclosed'. */
    if (pids == 0 || pids[fdes = fileno(iop)] == 0)
        return (-1);
    fclose(iop);
#ifdef HAVE_SIGEMPTYSET
    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);
    sigaddset(&sig,SIGQUIT);
    sigaddset(&sig, SIGHUP);
    sigprocmask( SIG_BLOCK, &sig, &omask);
#else
    omask = sigblock(sigmask(SIGINT) | sigmask(SIGQUIT) | sigmask(SIGHUP));
#endif

    while ((pid = wait(&stat_loc)) != pids[fdes] && pid != -1) ;
    pids[fdes] = 0;
#ifdef SIGCHLD
    signal(SIGCHLD, SIG_IGN);
#endif
#ifdef HAVE_SIGEMPTYSET
    sigprocmask( SIG_SETMASK, &omask, (sigset_t *)NULL);
    return(pid == -1 ? -1 : WEXITSTATUS(stat_loc));
#elif defined(NeXT)
    sigsetmask(omask);
    return (pid == -1 ? -1 : stat_loc.w_status);
#else
    sigsetmask(omask);
    return (pid == -1 ? -1 : stat_loc);
#endif
}



