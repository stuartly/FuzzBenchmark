/*
 * Copyright (c) 1985 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "config.h"

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)$Id: getusershell.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";

#endif /* LIBC_SCCS and not lint */

#ifndef _AIX

#include "../src/config.h"
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


#define SHELLS "/etc/shells"

/*
 * Do not add local shells here.  They should be added in /etc/shells
 */
static char *okshells[] =
{"/bin/sh", "/bin/csh", 0};

static char **shells,
 *strings;
static char **curshell = NULL;
static char **initshells();

/*
 * Get a list of shells from SHELLS, if it exists.
 */
char *getusershell()
{
	char *ret;

	if (curshell == NULL)
		curshell = initshells();
	ret = *curshell;
	if (ret != NULL)
		curshell++;
	return(ret);
}

void endusershell()
{
	if (shells != NULL)
		free((char *) shells);
	shells = NULL;
	if (strings != NULL)
		free(strings);
	strings = NULL;
	curshell = NULL;
}

void setusershell()
{
	curshell = initshells();
}

static char ** initshells()
{
	register char **sp,
	 *cp;
	register FILE *fp;
	struct stat statb;

	if (shells != NULL)
		free((char *) shells);
	shells = NULL;
	if (strings != NULL)
		free(strings);
	strings = NULL;
	if ((fp = fopen(SHELLS, "r")) == (FILE *) 0)
		return (okshells);
	if (fstat(fileno(fp), &statb) == -1) {
		fclose(fp);
		return (okshells);
	}
	if ((strings = (char *) malloc((unsigned) statb.st_size + 1)) == NULL) {
		fclose(fp);
		return (okshells);
	}
	shells = (char **) calloc((unsigned) statb.st_size / 3, sizeof(char *));

	if (shells == NULL) {
		fclose(fp);
		free(strings);
		strings = NULL;
		return (okshells);
	}
	sp = shells;
	cp = strings;
	while (fgets(cp, MAXPATHLEN + 1, fp) != NULL) {
		while (*cp != '#' && *cp != '/' && *cp != '\0')
			cp++;
		if (*cp == '#' || *cp == '\0')
			continue;
		*sp++ = cp;
		while (!isspace(*cp) && *cp != '#' && *cp != '\0')
			cp++;
		*cp++ = '\0';
	}
	*sp = (char *) 0;
	fclose(fp);
	return (shells);
}

#else /* it is AIX */
		 

/* emulate getusershell for AIX */

#include <userconf.h>
#include <usersec.h>

static int GETUSERSHELL_opened=0;
static char **GETUSERSHELL_shells;
static int GETUSERSHELL_current;
			  

char *getusershell()
{		
	static char *val;
	static char *list;
	static char *retVal;
	int n;
					
	if (!GETUSERSHELL_opened)
	{
		if(getconfattr(SC_SYS_LOGIN,SC_SHELLS,(void *)&val,SEC_LIST))
		{
			return(NULL);
		}
		GETUSERSHELL_opened = 1;
		GETUSERSHELL_current = 0;
		list = val;
	}	   
				   
	if ( (list != NULL) && (*list != NULL) )
	{
	while (list && *list)
		list++;
		
	*list = '\0';		
	
	retVal = val;
	list++; 
	val = list;
	}
	else
		retVal = NULL;
	
	return(retVal);
	
}

void setusershell()
{
	GETUSERSHELL_opened = 0;
}

void endusershell()
{
	GETUSERSHELL_opened = 0;
}

	
				   
	
	
	
	
#endif
