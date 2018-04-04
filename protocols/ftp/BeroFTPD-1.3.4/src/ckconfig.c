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
static char *rcsid = "$Id: ckconfig.c,v 1.6 1997/12/21 23:02:03 sob beta16 $";

#include "config.h"
#ifndef HOST_ACCESS
#define  HOST_ACCESS  1
#endif
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "pathnames.h"

int main()
{
  struct stat  sbuf;
  char        *sp;
  char         buf[1024];

#ifdef VIRTUAL
  FILE        *svrfp;
  char        accesspath[1024];
  char        hostaddress[32];
  int         read_servers_line();
#endif 

  /* _PATH_FTPUSERS   */
  fprintf(stdout, "Checking _PATH_FTPUSERS :: %s\n", _PATH_FTPUSERS);
  if ( (stat(_PATH_FTPUSERS, &sbuf)) < 0 )
    printf("I can't find it... look in doc/examples for an example.\n");
  else
    printf("ok.\n");

#ifdef VIRTUAL

  /* _PATH_FTPSERVERS  */
  fprintf(stdout, "\nChecking _PATH_FTPSERVERS :: %s\n", _PATH_FTPSERVERS);
  if ( (stat(_PATH_FTPSERVERS, &sbuf)) < 0 )
    printf("I can't find it... look in doc/examples for an example.\n");
  else {
    printf("ok.\n");
    /* Need to check the access files specified in the ftpservers file. */
    if ((svrfp = fopen(_PATH_FTPSERVERS, "r")) == NULL) 
       printf("I can't open it! check permissions and run ckconfig again.\n");
    else {
       while (read_servers_line(svrfp, hostaddress, accesspath, 0) == 1) {
         fprintf(stderr, "\nChecking accessfile for %s :: %s\n", hostaddress, accesspath);
         /*
         ** check to see that a valid directory value was
         ** supplied and not something such as "INTERNAL"
         **
         ** It is valid to have a string such as "INTERNAL" in the
         ** ftpservers entry. This is not an error. Silently ignore it.
         */
         if (stat(accesspath, &sbuf) == 0) {
             if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
                 printf("ok.\n");
             else {
                 printf("Check servers file and make sure only directories are listed...\n");
                 printf("look in doc/examples for an example.\n");
             }
         }
         else 
            printf("Internal ftpaccess usage specified... ok.\n");
       }
       fclose(svrfp);
    }
  }
#endif 

  /* _PATH_FTPACCESS  */
  fprintf(stdout, "\nChecking _PATH_FTPACCESS :: %s\n", _PATH_FTPACCESS);
  if ( (stat(_PATH_FTPACCESS, &sbuf)) < 0 )
    printf("I can't find it... look in doc/examples for an example.\n");
  else
    printf("ok.\n");

  /* _PATH_PIDNAMES   */
  fprintf(stdout, "\nChecking _PATH_PIDNAMES :: %s\n", _PATH_PIDNAMES);
  strcpy(buf, _PATH_PIDNAMES);
  sp = (char *)strrchr(buf, '/');
  *sp = '\0';
  if ( (stat(buf, &sbuf)) < 0 ) {
    printf("I can't find it...\n");
    printf("You need to make this directory [%s] in order for\n",buf);
    printf("the limit and user count functions to work.\n");
  } else
    printf("ok.\n");

  /* _PATH_CVT        */
  fprintf(stdout, "\nChecking _PATH_CVT :: %s\n", _PATH_CVT);
  if ( (stat(_PATH_CVT, &sbuf)) < 0 )
    printf("I can't find it... look in doc/examples for an example.\n");
  else
    printf("ok.\n");

  /* _PATH_XFERLOG    */
  fprintf(stdout, "\nChecking _PATH_XFERLOG :: %s\n", _PATH_XFERLOG);
  if ( (stat(_PATH_XFERLOG, &sbuf)) < 0 ) {
    printf("I can't find it... \n");
    printf("Don't worry, it will be created automatically by the\n");
    printf("server if you do transfer logging.\n");
  } else
    printf("ok.\n");

  /* _PATH_PRIVATE    */
  fprintf(stdout, "\nChecking _PATH_PRIVATE :: %s\n", _PATH_PRIVATE);
  if ( (stat(_PATH_PRIVATE, &sbuf)) < 0 ) {
    printf("I can't find it... look in doc/examples for an example.\n");
    printf("You only need this if you want SITE GROUP and SITE GPASS\n");
    printf("functionality. If you do, you will need to edit the example.\n");
  } else
    printf("ok.\n");

  /* _PATH_FTPHOSTS   */
  fprintf(stdout, "\nChecking _PATH_FTPHOSTS :: %s\n", _PATH_FTPHOSTS);
  if ( (stat(_PATH_FTPHOSTS, &sbuf)) < 0 ) {
    printf("I can't find it... look in doc/examples for an example.\n");
    printf("You only need this if you are using the HOST ACCESS features\n");
    printf("of the server.\n");
  } else
    printf("ok.\n");
  return(0);
}
