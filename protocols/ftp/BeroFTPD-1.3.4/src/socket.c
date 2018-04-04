/* socket handling
 * based on BeroList-2.3.0+ SMTP handling
 * based on the socket handling routines from fetchmail-2.6
 */

#ifndef lint
static char sccsid[] = "@(#)$Id: socket.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";
#endif /* not lint */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif
#include "socket.h"
#include "tool.h"

/*
 * There  are, in effect, two different implementations here.  One
 * uses read(2) and write(2) directly with no buffering, the other
 * uses stdio with line buffering (for better throughput).  Both
 * are known to work under Linux.
 */
#define BUFFER_SOCKET

#ifdef BUFFER_SOCKET
	/*
	 * Size of buffer for internal buffering read function
	 * don't increase beyond the maximum atomic read/write size for
	 * your sockets, or you'll take a potentially huge performance hit
	 */
	#define  INTERNAL_BUFSIZE       2048
#endif /* BUFFER_SOCKET */

FILE *SockOpen(char *host, int clientPort)
{
	int sock;
	unsigned long inaddr;
	struct sockaddr_in ad;
	struct hostent *hp;
	#ifdef BUFFER_SOCKET
		FILE *fp;
	#endif

	memset(&ad, 0, sizeof(ad));
	ad.sin_family = AF_INET;

	inaddr = inet_addr(host);
	if (inaddr != (unsigned long) -1)
		memcpy(&ad.sin_addr, &inaddr, sizeof(inaddr));
	else {
		hp = gethostbyname(host);
		if (hp == NULL)
			return (FILE *)NULL;
		memcpy(&ad.sin_addr, hp->h_addr, hp->h_length);
	}
	ad.sin_port = htons(clientPort);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return (FILE *)NULL;
	if (connect(sock, (struct sockaddr *) &ad, sizeof(ad)) < 0) {
		close(sock);
		return (FILE *)NULL;
	}
	#ifndef BUFFER_SOCKET
		return fdopen(sock, "r+");
	#else
		fp = fdopen(sock, "r+");
		setvbuf(fp, NULL, _IOLBF, INTERNAL_BUFSIZE);
		return(fp);
	#endif
}

int SockPrintf(FILE *sockfp, char* format, ...)
{
	va_list ap;
	char buf[32768];

	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	return SockWrite(buf, 1, slen(buf), sockfp);
}

#ifndef BUFFER_SOCKET

	int SockWrite(char *buf, int size, int len, FILE *sockfp)
	{
		int n, wrlen = 0;

		len *= size;
		while (len) {
			n = write(fileno(sockfp), buf, len);
			if (n <= 0)
				return -1;
			len -= n;
			wrlen += n;
			buf += n;
		}
		return wrlen;
	}

	char *SockGets(FILE *sockfp, char *buf, int len)
	{
		int rdlen = 0;
		char *cp = buf;

		while (--len) {
			if (read(fileno(sockfp), cp, 1) != 1)
				return((char *)NULL);
			else
				rdlen++;
			if (*cp++ == '\n')
				break;
		}
		*cp = 0;
		return buf;
	}

#else

	int SockWrite(char *buf, int size, int len, FILE *sockfp)
	{
		return(fwrite(buf, size, len, sockfp));
	}

	char *SockGets(FILE *sockfp, char *buf, int len)
	{
		return(fgets(buf, len, sockfp));
	}

#endif

int SockPuts(FILE *sockfp, char *buf)
{
	int rc;

	if ((rc = SockWrite(buf, 1, slen(buf), sockfp)))
		return rc;
	return SockWrite("\r\n", 1, 2, sockfp);
}

/* Reply: wait for RFC-number return code */

int Reply(FILE *sockfp)
{
	char *reply, *rec, *separator;
	int ret=0;

	reply=salloc(1024);
	do {
		rec=SockGets(sockfp,reply,1024);
		if(rec!=NULL) {
			ret=strtol(reply,&separator,10);
		} else
			#ifndef SOCKGETS_MAY_NOT_RETURN_NULL
				ret=250;
			#else
				abandon(13,"ERROR - SockGets() returned NULL - contact list operator.");
			#endif
	} while((rec!=NULL)&&(separator[0]!=' '));
	return ret;
}

int Send(FILE *sockfp, char *format, ...)
{
	va_list ap;
	char buf[32728];

	va_start(ap,format);
	vsprintf(buf,format,ap);
	va_end(ap);
	SockWrite(buf,1,slen(buf),sockfp);
	return Reply(sockfp);
}
