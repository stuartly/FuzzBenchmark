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
static char rcsid[] = "@(#)$Id: private.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";
#endif /* not lint */

#ifndef NO_PRIVATE

#include "config.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif
#include <grp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "pathnames.h"
#include "extensions.h"

#define MAXGROUPLEN 100
char *passbuf = NULL;
char groupname[MAXGROUPLEN];
int group_given = 0;

struct acgrp {
    char  gname[MAXGROUPLEN];    /* access group name */
    char  gpass[MAXGROUPLEN];    /* access group password */
    char  gr_name[MAXGROUPLEN];  /* group to setgid() to */
	gid_t gr_gid;
	struct acgrp *next;
};

struct acgrp *privptr;

extern int lgi_failure_threshold,
  autospout_free;
extern char remotehost[],
  remoteaddr[],
  remoteident[],
 *autospout;
int group_attempts;

void parsepriv()
{
    char *ptr;
    char *acptr = passbuf,
     *line;
    char *argv[3],
     *p,
     *val;
    struct acgrp *aptr,
     *privtail = (struct acgrp *) NULL;
    struct group *gr;
    int n;

    if (!passbuf || !(*passbuf))
        return;

    /* read through passbuf, stripping comments. */
    while (*acptr != '\0') {
        line = acptr;
        while (*acptr && *acptr != '\n')
            acptr++;
        *acptr++ = '\0';

        /* deal with comments */
        if ((ptr = strchr(line, '#')) != NULL)
            *ptr = '\0';

        if (*line == '\0')
            continue;

        /* parse the lines... */
        for (n = 0, p = line; n < 3 && p != NULL; n++) {
            val = (char *) strsep(&p, ":\n");
            argv[n] = val;
            if ((argv[n][0] == ' ') || (argv[n][0] == '\0'))
                argv[n] = NULL;
        }
        /* check their were 3 fields, if not skip the line... */
        if (n != 3 || p != NULL)
            continue;

if (argv[0] && argv[2]) {
if (argv[2][0] == '%') {
  gid_t gid = atoi (argv[2]+1);
        if ((gr = getgrgid(gid)) != NULL)  {
            aptr = (struct acgrp *) calloc(1, sizeof(struct acgrp));

            /* add element to end of list */
            if (privtail)
                privtail->next = aptr;
            privtail = aptr;
            if (!privptr)
                privptr = aptr;

            strcpy(aptr->gname, (char *) argv[0]);
if (argv[1] == NULL) aptr->gpass[0] = '\0'; else
            strcpy(aptr->gpass, (char *) argv[1]);
            strcpy(aptr->gr_name, gr->gr_name);
            aptr->gr_gid = gid;
        }
} else {
        if ((gr = getgrnam((char *) argv[2])) != NULL)  {
            aptr = (struct acgrp *) calloc(1, sizeof(struct acgrp));
            
            /* add element to end of list */
            if (privtail)
                privtail->next = aptr;
            privtail = aptr;
            if (!privptr)
                privptr = aptr;

            strcpy(aptr->gname, (char *) argv[0]);
if (argv[1] == NULL) aptr->gpass[0] = '\0'; else
            strcpy(aptr->gpass, (char *) argv[1]);
            strcpy(aptr->gr_name, (char *) argv[2]);
            aptr->gr_gid = gr->gr_gid;
        }
}
	endgrent();
}
    }
}

/*************************************************************************/
/* FUNCTION  : priv_setup                                                */
/* PURPOSE   : Set things up to use the private access password file.    */
/* ARGUMENTS : path, the path to the private access password file        */
/*************************************************************************/

void priv_setup(char *path)
{
    FILE *prvfile;
    struct stat finfo;

    passbuf = (char *) NULL;

    if ((prvfile = fopen(path, "r")) == NULL) {
        if (errno != ENOENT)
            syslog(LOG_ERR, "cannot open private access file %s: %s",
                   path, strerror(errno));
        return;
    }
    if (fstat(fileno(prvfile), &finfo) != 0) {
        syslog(LOG_ERR, "cannot fstat private access file %s: %s", path,
               strerror(errno));
        (void) fclose(prvfile);
        return;
    }
    if (finfo.st_size == 0) {
        passbuf = (char *) calloc(1, 1);
    } else {
        if (!(passbuf = (char *)malloc((unsigned) finfo.st_size + 1))) {
            (void) syslog(LOG_ERR, "could not malloc passbuf (%d bytes)",
                          finfo.st_size + 1);
            (void) fclose(prvfile);
            return;
        }
        if (!fread(passbuf, (size_t) finfo.st_size, 1, prvfile)) {
            (void) syslog(LOG_ERR, "error reading private access file %s: %s",
                          path, strerror(errno));
            (void) fclose(prvfile);
            return;
        }
        *(passbuf + finfo.st_size) = '\0';
    }
    (void) fclose(prvfile);
    (void) parsepriv();
}

/*************************************************************************/
/* FUNCTION  : priv_getent                                               */
/* PURPOSE   : Retrieve an entry from the in-memory copy of the group    */
/* access file.                                              */
/* ARGUMENTS : pointer to group name                                     */
/*************************************************************************/

struct acgrp * priv_getent(char *group)
{
	struct acgrp *ptr;

	for (ptr = privptr; ptr; ptr=ptr->next)
		if (!strcasecmp(group, ptr->gname))
			return(ptr);

    return (NULL);
}

/*************************************************************************/
/* FUNCTION  : priv_group                                                */
/* PURPOSE   :                                                           */
/* ARGUMENTS :                                                           */
/*************************************************************************/

void priv_group(char *group)
{
    if ((int)strlen(group) < MAXGROUPLEN) {
        strncpy(groupname, group, MAXGROUPLEN);
        group_given = 1;
        reply(200, "Request for access to group %s accepted.", group);
    } else {
        group_given = 0;
        reply(500, "Illegal group name");
    }

}

/*************************************************************************/
/* FUNCTION  : priv_gpass                                                */
/* PURPOSE   : validate the group access request, and if OK place user   */
/* in the proper group.                                      */
/* ARGUMENTS : group access password                                     */
/*************************************************************************/

void priv_gpass(char *gpass)
{
    char *xgpass = NULL;
#ifndef NO_CRYPT_PROTO
    char *crypt(const char *, const char *);
#endif
    struct acgrp *grp;
    struct group *gr;
    uid_t uid;
    gid_t gid;

    if (group_given == 0) {
        reply(503, "Give group name with SITE GROUP first.");
        return;
    }
    /* OK, now they're getting a chance to specify a password.  Make them
     * give the group name again if they fail... */
    group_given = 0;

    grp = priv_getent(groupname);
    if (passbuf && gpass && *gpass != '\0' && grp && *grp->gpass != '\0')
        xgpass = crypt(gpass, grp->gpass);

    if (!(   (   (gpass       != NULL)
              && (*gpass      != '\0')
              && (grp         != NULL)
              && (*grp->gpass != '\0')
              && (strcmp (xgpass, grp->gpass) == 0))
          || (   (   (gpass  == NULL)
                  || (*gpass == '\0'))
              && (grp         != NULL)
              && (*grp->gpass == '\0'))
       )) {
        reply(530, "Group access request incorrect.");
        grp = NULL;
        if (++group_attempts >= lgi_failure_threshold) {
            syslog(LOG_NOTICE,
                   "repeated group access failures from %s, group %s",
                   remoteident, groupname);
            exit(0);
        }
        sleep(group_attempts);  /* slow down password crackers */
        return;
    }

    uid = geteuid();
    gid = grp->gr_gid;

    delay_signaling(); /* we can't allow any signals while euid==0: kinch */
    seteuid(0);
#ifdef HAVE_SETEGID
    setegid(gid);
#else
    setgid(gid);
#endif
    seteuid(uid);
    enable_signaling(); /* we can allow signals once again: kinch */

    reply(200, "Group access enabled.");
    group_attempts = 0;
}
#endif /* !NO_PRIVATE */
