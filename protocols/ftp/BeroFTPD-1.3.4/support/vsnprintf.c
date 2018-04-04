/*
 * vsnprintf hack 
 * better than raw vsprintf, but only just barely....
 * $Id: vsnprintf.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $
 */

#include "config.h"

#include <stdio.h>
#include <varargs.h>
int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	char *str;
	int status;
	if (bufsize < 1)
	  return(EOF);
	/* we need to verify that the actual length of the buf is greater than
           or equal to the bufsize argument */
        if ((str = (char *)calloc(bufsize+1))!= NULL){
	  strncpy(str,buf,bufsize);
	  status = vsprintf(str, fmt, ap);  
	  strcpy(buf,str);
	  free(str);
	}else
	  status = vsprintf(buf, fmt, ap);
        return status;
}
