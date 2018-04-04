/* Originally taken from FreeBSD 3.0's libc; adapted to handle chroot
 * directories in BeroFTPD by Bernhard Rosenkraenzer
 * <bero@beroftpd.unix.eu.org>
 *
 * Added super-user permissions so we can determine the real pathname even
 * if the user cannot access the file. <lundberg+wuftpd@vr.net>
 *
 * Copyright (c) 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)realpath.c	8.1 (Berkeley) 2/16/94";
#endif /* LIBC_SCCS and not lint */

#include "config.h"

#include <sys/param.h>
#include <sys/stat.h>

#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <string.h>

#ifndef MAXSYMLINKS  /* Workaround for Linux libc 4.x/5.x */
#define MAXSYMLINKS 5
#endif

char *fb_realpath(const char *path, char *resolved);

char *wu_realpath(const char *path, char resolved_path[MAXPATHLEN], const char *chroot_path)
{
	char *ptr;
	char q[MAXPATHLEN];

	fb_realpath(path,q);

	if(chroot_path==NULL)
		strcpy(resolved_path,q);
	else {
		strcpy(resolved_path, chroot_path);
		if (q[0]!='/') {
			if(strlen(resolved_path)+strlen(q)<MAXPATHLEN)
				strcat(resolved_path, q);
			else /* Avoid buffer overruns... */
				return NULL;
		} else if (q[1] != '\0') {
			for (ptr=q; *ptr!= '\0'; ptr++);
			if (ptr==resolved_path || *--ptr != '/') {
				if(strlen(resolved_path)+strlen(q)<MAXPATHLEN)
					strcat(resolved_path, q);
				else /* Avoid buffer overruns... */
					return NULL;
			} else {
				if(strlen(resolved_path)+strlen(q)-1<MAXPATHLEN)
					strcat(resolved_path, &q[1]);
				else /* Avoid buffer overruns... */
					return NULL;
			}
		}
	}
	return resolved_path;
}

/*
 * char *fb_realpath(const char *path, char resolved_path[MAXPATHLEN]);
 *
 * Find the real name of path, by removing all ".", ".." and symlink
 * components.  Returns (resolved) on success, or (NULL) on failure,
 * in which case the path which caused trouble is left in (resolved).
 */
char *fb_realpath(const char *path, char *resolved)
{
	struct stat sb;
	int fd, n, rootd, serrno;
	char *p, *q, wbuf[MAXPATHLEN];
        int symlinks = 0;
        uid_t userid;
        int resultcode;
#ifndef HAVE_FCHDIR
        char cwd[MAXPATHLEN+1];
        char *pcwd;
#endif

	/* Save the starting point. */
        userid = geteuid();
        delay_signaling();
        seteuid(0);
#ifndef HAVE_FCHDIR
#ifdef HAVE_GETCWD
        pcwd = getcwd(cwd, sizeof(cwd));
#else
        pcwd = getwd(cwd);
#endif
#else
        fd = open(".", O_RDONLY);
#endif
        seteuid(userid);
        enable_signaling();
#ifndef HAVE_FCHDIR
        if (pcwd == NULL) {
#else
        if (fd < 0) {
#endif        
		(void)strcpy(resolved, ".");
		return (NULL);
	}

	/*
	 * Find the dirname and basename from the path to be resolved.
	 * Change directory to the dirname component.
	 * lstat the basename part.
	 *     if it is a symlink, read in the value and loop.
	 *     if it is a directory, then change to that directory.
	 * get the current directory name and append the basename.
	 */
	(void)strncpy(resolved, path, MAXPATHLEN - 1);
	resolved[MAXPATHLEN - 1] = '\0';
loop:
	q = strrchr(resolved, '/');
	if (q != NULL) {
		p = q + 1;
		if (q == resolved)
			q = "/";
		else {
			do {
				--q;
			} while (q > resolved && *q == '/');
			q[1] = '\0';
			q = resolved;
		}
		delay_signaling();
		seteuid(0);
		resultcode = chdir(q);
		seteuid(userid);
		enable_signaling();
		if (resultcode < 0)
			goto err1;
	} else
		p = resolved;

	/* Deal with the last component. */
	if (*p != '\0') {
	    delay_signaling();
	    seteuid(0);
	    resultcode = lstat(p, &sb);
	    seteuid(userid);
	    enable_signaling();
	    if (resultcode == 0) {
		if (S_ISLNK(sb.st_mode)) {
                      if (++symlinks > MAXSYMLINKS) {
                              errno = ELOOP;
                              goto err1;
                      }
                        delay_signaling();
                        seteuid(0);                       
			n = readlink(p, resolved, MAXPATHLEN);
                        seteuid(userid);
                        enable_signaling();
			if (n < 0)
				goto err1;
			resolved[n] = '\0';
			goto loop;
		}
		if (S_ISDIR(sb.st_mode)) {
			delay_signaling();
			seteuid(0);
			resultcode = chdir(p);
			seteuid(userid);
			enable_signaling();
			if (resultcode < 0)
				goto err1;
			p = "";
		}
	    }
	}

	/*
	 * Save the last component name and get the full pathname of
	 * the current directory.
	 */
	strcpy(wbuf, p);
	delay_signaling();
	seteuid(0);
#ifdef HAVE_GETCWD
	resultcode = getcwd(resolved, MAXPATHLEN) == NULL ? 0 : 1;
#else
        resultcode = getwd(resolved) == NULL ? 0 : 1;
        if (resolved[MAXPATHLEN -1 ] != '\0') {
            resultcode = 0;
            errno = ERANGE;
        }
#endif
        seteuid(userid);
        enable_signaling();
        if (resultcode == 0)
		goto err1;

	/*
	 * Join the two strings together, ensuring that the right thing
	 * happens if the last component is empty, or the dirname is root.
	 */
	if (resolved[0] == '/' && resolved[1] == '\0')
		rootd = 1;
	else
		rootd = 0;

	if (*wbuf) {
		if (strlen(resolved) + strlen(wbuf) + rootd + 1 > MAXPATHLEN) {
			errno = ENAMETOOLONG;
			goto err1;
		}
		if (rootd == 0)
			(void)strcat(resolved, "/");
		(void)strcat(resolved, wbuf);
	}

	/* Go back to where we came from. */
	delay_signaling();
	seteuid(0);
#ifndef HAVE_FCHDIR
	resultcode = chdir(cwd);
#else
	resultcode = fchdir(fd);
#endif
	seteuid(userid);
	enable_signaling();
	if (resultcode < 0) {
		serrno = errno;
		goto err2;
	}

#ifdef HAVE_FCHDIR
	/* It's okay if the close fails, what's an fd more or less? */
	(void)close(fd);
#endif
	return (resolved);

err1:	serrno = errno;
	delay_signaling();
	seteuid(0);
#ifdef HAVE_FCHDIR
	fchdir(fd);
#else
        chdir(cwd);
#endif
	seteuid(userid);
	enable_signaling();
#ifdef HAVE_FCHDIR
err2:	(void)close(fd);
	errno = serrno;
#else
err2:	errno = serrno;
#endif
	return (NULL);
}
