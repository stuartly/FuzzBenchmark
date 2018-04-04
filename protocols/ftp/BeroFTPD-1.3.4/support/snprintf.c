/*
 * Copyright (c) 1983, 1995, 1996 Eric P. Allman
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
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
/* Extracted from sendmail 8.8.5 */

#include "config.h"

#ifndef lint
static char sccsid[] = "@(#)$Id: snprintf.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $ excerpted from conf.c 8.333 (Berkeley) 1/21/97";
#endif /* not lint */
#include <stdio.h>
#include <stdlib.h>

#ifndef __P
#define __P(p)  p
#endif

#include <stdarg.h>
#define VA_LOCAL_DECL	va_list ap;
#define VA_START(f)	va_start(ap, f)
#define VA_END		va_end(ap)

#ifdef SOLARIS2
#ifdef _FILE_OFFSET_BITS
#define SOLARIS26
#endif
#endif

#ifdef SOLARIS26
#define HAS_SNPRINTF
#define HAS_VSNPRINTF
#endif
#ifdef _SCO_DS_
#define HAS_SNPRINTF
#endif
#ifdef luna2
#define HAS_VSNPRINTF
#endif
/*
**  SNPRINTF, VSNPRINT -- counted versions of printf
**
**	These versions have been grabbed off the net.  They have been
**	cleaned up to compile properly and support for .precision and
**	%lx has been added.
*/

/**************************************************************
 * Original:
 * Patrick Powell Tue Apr 11 09:48:21 PDT 1995
 * A bombproof version of doprnt (dopr) included.
 * Sigh.  This sort of thing is always nasty do deal with.  Note that
 * the version here does not include floating point...
 *
 * snprintf() is used instead of sprintf() as it does limit checks
 * for string length.  This covers a nasty loophole.
 *
 * The other functions are there to prevent NULL pointers from
 * causing nast effects.
 **************************************************************/

/*static char _id[] = "$Id: snprintf.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";*/
static void dopr();
static char *end;
#ifndef HAS_SNPRINTF
/* VARARGS3 */
int snprintf(char *str, size_t count, const char *fmt, ...)
{
	int len;
	VA_LOCAL_DECL

	VA_START(fmt);
	len = vsnprintf(str, count, fmt, ap);
	VA_END;
	return len;
}
#endif 

#ifndef HAS_VSNPRINTF
int vsnprintf(char *str, size_t count, const char *fmt, va_list args)
{
	str[0] = 0;
	end = str + count - 1;
	dopr( str, fmt, args );
	if (count > 0)
		end[0] = 0;
	return strlen(str);
}

/*
 * dopr(): poor man's version of doprintf
 */

static void fmtstr __P((char *value, int ljust, int len, int zpad, int maxwidth));
static void fmtnum __P((long value, int base, int dosign, int ljust, int len, int zpad));
static void dostr __P(( char * , int ));
static char *output;
static void dopr_outch __P(( int c ));

static void dopr(char  * buffer, const char * format, va_list args )
{
       int ch;
       long value;
       int longflag  = 0;
       int pointflag = 0;
       int maxwidth  = 0;
       char *strvalue;
       int ljust;
       int len;
       int zpad;

       output = buffer;
       while( (ch = *format++) ){
               switch( ch ){
               case '%':
                       ljust = len = zpad = maxwidth = 0;
                       longflag = pointflag = 0;
               nextch:
                       ch = *format++;
                       switch( ch ){
                       case 0:
                               dostr( "**end of format**" , 0);
                               return;
                       case '-': ljust = 1; goto nextch;
                       case '0': /* set zero padding if len not set */
                               if(len==0 && !pointflag) zpad = '0';
                       case '1': case '2': case '3':
                       case '4': case '5': case '6':
                       case '7': case '8': case '9':
			       if (pointflag)
				 maxwidth = maxwidth*10 + ch - '0';
			       else
				 len = len*10 + ch - '0';
                               goto nextch;
		       case '*': 
			       if (pointflag)
				 maxwidth = va_arg( args, int );
			       else
				 len = va_arg( args, int );
			       goto nextch;
		       case '.': pointflag = 1; goto nextch;
                       case 'l': longflag = 1; goto nextch;
                       case 'u': case 'U':
                               /*fmtnum(value,base,dosign,ljust,len,zpad) */
                               if( longflag ){
                                       value = va_arg( args, long );
                               } else {
                                       value = va_arg( args, int );
                               }
                               fmtnum( value, 10,0, ljust, len, zpad ); break;
                       case 'o': case 'O':
                               /*fmtnum(value,base,dosign,ljust,len,zpad) */
                               if( longflag ){
                                       value = va_arg( args, long );
                               } else {
                                       value = va_arg( args, int );
                               }
                               fmtnum( value, 8,0, ljust, len, zpad ); break;
                       case 'd': case 'D':
                               if( longflag ){
                                       value = va_arg( args, long );
                               } else {
                                       value = va_arg( args, int );
                               }
                               fmtnum( value, 10,1, ljust, len, zpad ); break;
                       case 'x':
                               if( longflag ){
                                       value = va_arg( args, long );
                               } else {
                                       value = va_arg( args, int );
                               }
                               fmtnum( value, 16,0, ljust, len, zpad ); break;
                       case 'X':
                               if( longflag ){
                                       value = va_arg( args, long );
                               } else {
                                       value = va_arg( args, int );
                               }
                               fmtnum( value,-16,0, ljust, len, zpad ); break;
                       case 's':
                               strvalue = va_arg( args, char *);
			       if (maxwidth > 0 || !pointflag) {
				 if (pointflag && len > maxwidth)
				   len = maxwidth; /* Adjust padding */
				 fmtstr( strvalue,ljust,len,zpad, maxwidth);
			       }
			       break;
                       case 'c':
                               ch = va_arg( args, int );
                               dopr_outch( ch ); break;
                       case '%': dopr_outch( ch ); continue;
                       default:
                               dostr(  "???????" , 0);
                       }
                       break;
               default:
                       dopr_outch( ch );
                       break;
               }
       }
       *output = 0;
}

static void
fmtstr(  value, ljust, len, zpad, maxwidth )
       char *value;
       int ljust, len, zpad, maxwidth;
{
       int padlen, strlen;     /* amount to pad */

       if( value == 0 ){
               value = "<NULL>";
       }
       for( strlen = 0; value[strlen]; ++ strlen ); /* strlen */
       if (strlen > maxwidth && maxwidth)
	 strlen = maxwidth;
       padlen = len - strlen;
       if( padlen < 0 ) padlen = 0;
       if( ljust ) padlen = -padlen;
       while( padlen > 0 ) {
               dopr_outch( ' ' );
               --padlen;
       }
       dostr( value, maxwidth );
       while( padlen < 0 ) {
               dopr_outch( ' ' );
               ++padlen;
       }
}

static void
fmtnum(  value, base, dosign, ljust, len, zpad )
       long value;
       int base, dosign, ljust, len, zpad;
{
       int signvalue = 0;
       unsigned long uvalue;
       char convert[20];
       int place = 0;
       int padlen = 0; /* amount to pad */
       int caps = 0;

       /* DEBUGP(("value 0x%x, base %d, dosign %d, ljust %d, len %d, zpad %d\n",
               value, base, dosign, ljust, len, zpad )); */
       uvalue = value;
       if( dosign ){
               if( value < 0 ) {
                       signvalue = '-';
                       uvalue = -value;
               }
       }
       if( base < 0 ){
               caps = 1;
               base = -base;
       }
       do{
               convert[place++] =
                       (caps? "0123456789ABCDEF":"0123456789abcdef")
                        [uvalue % (unsigned)base  ];
               uvalue = (uvalue / (unsigned)base );
       }while(uvalue);
       convert[place] = 0;
       padlen = len - place;
       if( padlen < 0 ) padlen = 0;
       if( ljust ) padlen = -padlen;
       /* DEBUGP(( "str '%s', place %d, sign %c, padlen %d\n",
               convert,place,signvalue,padlen)); */
       if( zpad && padlen > 0 ){
               if( signvalue ){
                       dopr_outch( signvalue );
                       --padlen;
                       signvalue = 0;
               }
               while( padlen > 0 ){
                       dopr_outch( zpad );
                       --padlen;
               }
       }
       while( padlen > 0 ) {
               dopr_outch( ' ' );
               --padlen;
       }
       if( signvalue ) dopr_outch( signvalue );
       while( place > 0 ) dopr_outch( convert[--place] );
       while( padlen < 0 ){
               dopr_outch( ' ' );
               ++padlen;
       }
}

static void
dostr( str , cut)
     char *str;
     int cut;
{
  if (cut) {
    while(*str && cut-- > 0) dopr_outch(*str++);
  } else {
    while(*str) dopr_outch(*str++);
  }
}

static void
dopr_outch( c )
       int c;
{
#if 0
       if( iscntrl(c) && c != '\n' && c != '\t' ){
               c = '@' + (c & 0x1F);
               if( end == 0 || output < end )
                       *output++ = '^';
       }
#endif
       if( end == 0 || output < end )
               *output++ = c;
}

# endif

