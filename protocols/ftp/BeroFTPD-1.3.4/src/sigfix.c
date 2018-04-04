/* This software is Copyright 1997 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */
#ifndef lint
static char * rcsid = "$Id: sigfix.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";
#endif
#include "config.h"

 /*
  * delay_signaling(), enable_signaling - delay signal delivery for a while
  * 
  * Original Author: Wietse Venema with small changes by Dave Kinchlea and 
  * Stan Barber
  */

/* 
 * Some folks (notably those who do Linux hacking) say this fix is needed.
 * Others (notably the FreeBSD and BSDI folks) say if isn't.
 * I am making it possible to include or exclude it.
 * Just define NEED_SIGFIX and you get it.
 */
#ifdef NEED_SIGFIX
#include <sys/types.h>
#include <sys/signal.h>
#include <syslog.h>

static sigset_t saved_sigmask;
sigset_t block_sigmask;  /* used in ftpd.c */
static int delaying;
static int init_done;
#endif
/* enable_signaling - deliver delayed signals and disable signal delay */

int     enable_signaling()
{
#ifdef NEED_SIGFIX
    if (delaying != 0) {
        delaying = 0;
        if (sigprocmask(SIG_SETMASK, &saved_sigmask, (sigset_t *) 0) < 0) {
            syslog(LOG_ERR, "sigprocmask: %m");
            return (-1);
        }
    }
#endif
    return (0);
}

/* delay_signaling - save signal mask and block all signals */
int     delay_signaling()
{
#ifdef NEED_SIGFIX
    if (delaying == 0) {
        delaying = 1;
        if (sigprocmask(SIG_BLOCK, &block_sigmask, &saved_sigmask) < 0) {
            syslog(LOG_ERR, "sigprocmask: %m");
            return (-1);
        }
    }
#endif
    return (0);
}


