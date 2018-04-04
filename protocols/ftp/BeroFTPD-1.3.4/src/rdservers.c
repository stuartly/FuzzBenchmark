/*
 *    %W% %G%  -  Locating and opening the appropriate ftpaccess file.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * AUTHOR
 *      Kent Landfield  <kent@landfield.com>
 *
 * HISTORY
 *      970423  KBL      Created
 *	980930  Bero     Add support for NOT resolving hostname
 *                       (needed for supporting HOST command);
 *                       minor optimization.
 */

#include "config.h"

#ifdef  VIRTUAL

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

int read_servers_line(FILE *svrfp, char *hostaddress, char *accesspath, char resolve)
{
    struct hostent *hp;
    struct hostent *gethostbyname();
    char *inet_ntoa();

    static char buffer[BUFSIZ];
    char    *hcp, *acp;
    char    *bcp, *ecp;

    while (fgets(buffer, BUFSIZ, svrfp) != NULL) {

        /* Find first non-whitespace character */
        for (bcp = buffer; isspace(*bcp); bcp++) ;

        /* Get rid of comments */
        if ((ecp = strchr(buffer, '#')) != NULL)
             *ecp = '\0';

        /* Skip empty lines */
        if ((bcp == ecp) || (*bcp == '\n'))
             continue;

        /* separate parts */

        hcp = bcp;
        for (acp = hcp; 
             (*acp && !isspace(*acp)); acp++);

        /* better have something in access path or skip the line */
        if (!*acp)
            continue;
 
        *acp++ = '\0';
        
        while (*acp && isspace(*acp))
               acp++;

        /* again better have something in access path or skip the line */
        if (!*acp)
            continue;
 
        ecp = acp;

        while (*ecp && (!isspace(*ecp)) &&  *ecp != '\n')
               ++ecp;

        *ecp = '\0';

        if ((resolve!=0) && ((hp = gethostbyname(hcp)) != NULL)) {
           struct in_addr in; 
           memmove(&in, hp->h_addr, sizeof(in)); 
           strcpy(hostaddress, inet_ntoa(in)); 
        } else
            strcpy(hostaddress, hcp);

        if(acp[strlen(acp)-1]=='/')	/* remove trailing slash */
        	acp[strlen(acp)-1]=0;
        strcpy(accesspath, acp);
 
        return(1);
    }
    return(0);
}
#endif
