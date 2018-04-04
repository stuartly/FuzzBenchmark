/*
 * 5/6/91 DJB baseline authuser 3.1. Public domain.
 * $Id: authuser.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $
 */

#include "../src/config.h"
#include "config.h"

#ifdef USE_RFC931
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

extern int errno;

#include "authuser.h"

unsigned short auth_tcpport = 113;

#define SIZ 500					/* various buffers */

static int usercmp(register char *u, register char *v)
{
	/* is it correct to consider Foo and fOo the same user? yes */
	/* but the function of this routine may change later */
	while (*u && *v)
		if (tolower(*u) != tolower(*v))
			return tolower(*u) - tolower(*v);
		else
			++u, ++v;
	return *u || *v;
}

static char authline[SIZ];

char * auth_xline(register char *user, register int fd, register long unsigned int *in)
  /* the supposed name of the user, NULL if unknown */
  /* the file descriptor of the connection */
{
	unsigned short local;
	unsigned short remote;
	register char *ruser;

	if (auth_fd(fd, in, &local, &remote) == -1)
		return 0;
	ruser = auth_tcpuser(*in, local, remote);
	if (!ruser)
		return 0;
	if (!user)
		user = ruser;			/* forces X-Auth-User */
	sprintf(authline,
			(usercmp(ruser, user) ? "X-Forgery-By: %s" : "X-Auth-User: %s"),
				   ruser);
	return authline;
}

int auth_fd(register int fd, register long unsigned int *in, register short unsigned int *local, register short unsigned int *remote)
{
	struct sockaddr_in sa;
#if defined(UNIXWARE) || defined(AIX)
	size_t dummy;
#else
	int dummy;
#endif

	dummy = sizeof(sa);
	if (getsockname(fd, (struct sockaddr *)&sa, &dummy) == -1)
		return -1;
	if (sa.sin_family != AF_INET) {
		errno = EAFNOSUPPORT;
		return -1;
	}
	*local = ntohs(sa.sin_port);
	dummy = sizeof(sa);
	if (getpeername(fd, (struct sockaddr *)&sa, &dummy) == -1)
		return -1;
	*remote = ntohs(sa.sin_port);
	*in = sa.sin_addr.s_addr;
	return 0;
}

static char ruser[SIZ];
static char realbuf[SIZ];
static char *buf;

char * auth_tcpuser(register long unsigned int in, register short unsigned int local, register short unsigned int remote)
{
	struct sockaddr_in sa;
	register int s;
	register int buflen;
	register int w;
	register int saveerrno;
	char ch;
	unsigned short rlocal;
	unsigned short rremote;
	extern struct sockaddr_in ctrl_addr;
	int on = 1;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return 0;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on,sizeof(on));/*Try*/
	sa = ctrl_addr;
	sa.sin_port = htons(0);
	bind(s, (struct sockaddr *)&sa, sizeof(sa)); /* may as well try ... */
	sa.sin_family = AF_INET;
	sa.sin_port = htons(auth_tcpport);
	sa.sin_addr.s_addr = in;
	if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
		saveerrno = errno;
		close(s);
		errno = saveerrno;
		return 0;
	}
	buf = realbuf;
	sprintf(buf, "%u , %u\r\n", (unsigned int) remote, (unsigned int) local);
	/* note the reversed order---the example in the RFC is misleading */
	buflen = strlen(buf);
	while ((w = write(s, buf, buflen)) < buflen)
		if (w == -1) {			/* should we worry about 0 as well? */
			saveerrno = errno;
			close(s);
			errno = saveerrno;
			return 0;
		} else {
			buf += w;
			buflen -= w;
		}
	buf = realbuf;
	while ((w = read(s, &ch, 1)) == 1) {
		*buf = ch;
		if ((ch != ' ') && (ch != '\t') && (ch != '\r'))
			++buf;
		if ((buf - realbuf == sizeof(realbuf) - 1) || (ch == '\n'))
			break;
	}
	if (w == -1) {
		saveerrno = errno;
		close(s);
		errno = saveerrno;
		return 0;
	}
	*buf = '\0';

/* H* fix: limit scanf of returned identd string. */
	if (sscanf(realbuf, "%hd,%hd: USERID :%*[^:]:%400s",
			&rremote, &rlocal, ruser) < 3) {
		close(s);
		errno = EIO;
		/* makes sense, right? well, not when USERID failed to match
		   ERROR but there's no good error to return in that case */
		return 0;
	}
	if ((remote != rremote) || (local != rlocal)) {
		close(s);
		errno = EIO;
		return 0;
	}
	/* XXX: we're not going to do any backslash processing */
	close(s);
	return ruser;
}

#else /* USE_RFC931 */

int auth_dummy_variable = 0;    /* To keep compilers quiet */

#endif /* USE_RFC931 */
