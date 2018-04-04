/*
 * AUTHUSER include file
 * $Id: authuser.h,v 1.1.1.1 1998/08/21 18:10:34 root Exp $
 */
#ifndef AUTHUSER_H
#define AUTHUSER_H

extern unsigned short auth_tcpport;

#ifdef __STDC__
extern char *auth_xline(register char *user, register int fd, register long unsigned int *in);

extern int auth_fd(register int fd, register long unsigned int *in, register short unsigned int *local, register short unsigned int *remote);

extern char *auth_tcpuser(register long unsigned int in, register short unsigned int local, register short unsigned int remote);
#else
extern char *auth_xline();
extern int auth_fd();
extern char *auth_tcpuser();
#endif
#endif
