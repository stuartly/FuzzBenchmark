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
char * rcsid = "$Id: acl.c,v 1.1.1.1 1998/08/21 18:10:30 root Exp $";
#endif
#include "config.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#ifdef VIRTUAL
# include <netinet/in.h>
#endif

#include "pathnames.h"
#include "extensions.h"

static struct aclmember *aclmembers = NULL;
static struct aclmember *acltail = NULL;
#ifdef VIRTUAL
static struct aclmember *virtmembers = NULL;
static struct aclmember *virttail = NULL;
#endif

/* Have we read the ACL file yet? */
static int acl_read = 0;

#ifdef VIRTUAL	/* XXX this needs to be changed for IPv6 */
static long virtual_host_ip;
#endif

/*************************************************************************/
/* FUNCTION  : acl_setvirtual                                            */
/* PURPOSE   : Define the virtual host being used                        */
/* ARGUMENTS : pointer to the IP address (struct in_addr)                */
/* RETURNS   : nothing                                                   */
/*************************************************************************/

#ifdef VIRTUAL
void acl_setvirtual(struct in_addr *host)
{
    if (acl_read)
        syslog(LOG_DEBUG, "Trying to set virtual host after ACL file read");
    virtual_host_ip = host->s_addr;
}
#endif /* VIRTUAL */

/*************************************************************************/
/* FUNCTION  : getaclentry                                               */
/* PURPOSE   : Retrieve a named entry from the ACL                       */
/* ARGUMENTS : pointer to the keyword and a handle to the acl members    */
/* RETURNS   : pointer to the acl member containing the keyword or NULL  */
/*************************************************************************/

struct aclmember * getaclentry(char *keyword, struct aclmember **next)
{
#ifdef VIRTUAL
    /* If this is nonzero, we try searching the default list if we don't
     * find anything in the ACL for this virtual host.  We only want to
     * do this if there are no entries at all for the virtual host, so we
     * don't set it if *next is non-NULL (meaning we've already found
     * something).
     */
    int trydef = 0;
#endif

    do {
        if (!*next) {
#ifdef VIRTUAL
            if (!trydef) {
                *next = virtmembers;
                trydef = 1;
            } else {
#endif
                *next = aclmembers;
#ifdef VIRTUAL
                trydef = 0;
            }
#endif
        } else
            *next = (*next)->next;
    } while ((*next && strcasecmp((*next)->keyword, keyword))
#ifdef VIRTUAL
             || (!*next && trydef)
#endif
            );

    return (*next);
}

/*************************************************************************/
/* FUNCTION  : parseline (static)                                        */
/* PURPOSE   : Parse a single line into an ACL entry                     */
/* ARGUMENTS : Keyword for the entry                                     */
/*             strtok(NULL, "\t ") should return the first argument      */
/* RETURNS   : The struct aclmember * created (via malloc)               */
/*************************************************************************/

static struct aclmember * parseline(char *keyword)
{
    struct aclmember *member;
    int cnt;
    char *ptr;

    member = (struct aclmember *) malloc(sizeof(struct aclmember));
    memset(member, 0, sizeof(struct aclmember));
    strcpy(member->keyword, keyword);
    cnt = 0;
    while ((ptr = strtok(NULL, " \t")) != NULL) {
        if (cnt >= MAXARGS) {
            syslog(LOG_ERR,
                "Too many args (>%d) in ftpaccess: %s %s %s %s %s ...",
                MAXARGS - 1, member->keyword, member->arg[0],
                member->arg[1], member->arg[2], member->arg[3]);
            break;
        }
        member->arg[cnt++] = ptr;
    }
    return member;
}

/*************************************************************************/
/* FUNCTION  : parseacl (static)                                         */
/* PURPOSE   : Parse the acl buffer into its components                  */
/* ARGUMENTS : A pointer to the acl buffer                               */
/* RETURNS   : nothing                                                   */
/*************************************************************************/

static int parseacl(char *aclbuf)
{
    char *ptr,
     *aclptr = aclbuf,
     *line;
    int cnt;
    struct aclmember *member;
#ifdef VIRTUAL
    struct in_addr host;
#endif

    if (!aclbuf || !(*aclbuf))
        return (0);

    while (*aclptr != '\0') {
        line = aclptr;
        while (*aclptr && *aclptr != '\n')
            aclptr++;
        *aclptr++ = (char) NULL;

        /* deal with comments */
        if ((ptr = strchr(line, '#')) != NULL)
            /* allowed escaped '#' chars for path-filter (DiB) */
            if (*(ptr-1) != '\\')
                *ptr = '\0';

        ptr = strtok(line, " \t");
        if (ptr) {
            if (strcasecmp(ptr, "include") == 0) {
                ptr = strtok(NULL, " \t");
                if (ptr) {
                    if (!readacl(ptr))
                        syslog(LOG_ERR,
                            "Error including file %s in ftpaccess", ptr);
                } else {
                    syslog(LOG_ERR,
                        "Missing filename for `include' in ftpaccess");
                }
            } else if (strcasecmp(ptr, "virtual") == 0) {
#ifdef VIRTUAL
                ptr = strtok(NULL, " \t");
                if (!ptr) {
                    syslog(LOG_ERR,
                        "Missing IP address for `virtual' in ftpaccess");
                } else if ((host.s_addr = inet_addr(ptr)) == -1) {
                    syslog(LOG_ERR,
                        "Error parsing virtual IP address `%s'", ptr);
                } else if (!(ptr = strtok(NULL, " \t"))) {
                    syslog(LOG_ERR,
                        "Missing line after `virtual %s'",
                        inet_ntoa(host.s_addr));
                } else if (host.s_addr == virtual_host_ip) {
                    member = parseline(ptr);
                    if (virttail)
                        virttail->next = member;
                    virttail = member;
                    if (!virtmembers)
                        virtmembers = member;
                }
#else
		syslog(LOG_ERR, "`virtual' used in ftpaccess; this version has been compiled without support for virtual hosts.");
#endif
            } else {
                member = parseline(ptr);
                if (acltail)
                    acltail->next = member;
                acltail = member;
                if (!aclmembers)
                    aclmembers = member;
            }
        }
    }
    return (1);
}

/*************************************************************************/
/* FUNCTION  : readacl                                                   */
/* PURPOSE   : Read the acl into memory                                  */
/* ARGUMENTS : The pathname of the acl                                   */
/* RETURNS   : 0 if error, 1 if no error                                 */
/*************************************************************************/

int readacl(char *aclpath)
{
    FILE *aclfile;
    struct stat finfo;
    extern int use_accessfile;
    char *aclbuf;

    if (!use_accessfile)
        return (0);

    if ((aclfile = fopen(aclpath, "r")) == NULL) {
        syslog(LOG_ERR, "cannot open access file %s: %s", aclpath,
               strerror(errno));
        return (0);
    }
    if (fstat(fileno(aclfile), &finfo) != 0) {
        syslog(LOG_ERR, "cannot fstat access file %s: %s", aclpath,
               strerror(errno));
        fclose(aclfile);
        return (0);
    }
    if (finfo.st_size == 0)
        aclbuf = (char *) malloc(1);
    else {
        if (!(aclbuf = (char *)malloc((unsigned) finfo.st_size + 1))) {
            syslog(LOG_ERR, "could not malloc aclbuf (%d bytes)", finfo.st_size + 1);
            fclose(aclfile);
            return (0);
        }
        if (!fread(aclbuf, (size_t) finfo.st_size, 1, aclfile)) {
            syslog(LOG_ERR, "error reading acl file %s: %s", aclpath,
                   strerror(errno));
            aclbuf = NULL;
            fclose(aclfile);
            return (0);
        }
        *(aclbuf + finfo.st_size) = '\0';
    }
    fclose(aclfile);
    return (parseacl(aclbuf));
}
