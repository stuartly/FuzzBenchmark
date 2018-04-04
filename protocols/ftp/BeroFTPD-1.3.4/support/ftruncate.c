/*
 * $Id: ftruncate.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $
 * Using chsize on systems that support it (XENIX and SVR4)
 * SPECIAL NOTE: On Xenix and SVR4, using this call in the BSD library
 * will REQUIRE the use of -lx for the extended library since chsize()
 * is not in the standard C library.
 *
 */

#include "config.h"
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

ftruncate(fd,length)
    int fd;                     /* File descriptor for file that to change */
    off_t length;               /* New size for this file */
{
    int status;                 /* Status returned from chsize() proc */

    status = chsize(fd,length);
    return(status);
}

