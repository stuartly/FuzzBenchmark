/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "config.h"

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strerror.c	5.4 (Berkeley) 6/24/90";

#endif /* LIBC_SCCS and not lint */

#include "../src/config.h"

#include <string.h>

char *strerror(int errnum)
{
	extern int sys_nerr;
	extern char *sys_errlist[];
	static char ebuf[40];		/* 64-bit number + slop */

	if ((unsigned int) errnum < sys_nerr)
		return (sys_errlist[errnum]);
	sprintf(ebuf, "Unknown error: %d", errnum);
	return (ebuf);
}
