/* Copyright (c) 1985, 1988, 1990 Regents of the University of California.
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
 * University of California, Berkeley and its contributors. 4. Neither the
 * name of the University nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE. 
 */
#define ALTERNATE_CD

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1985, 1988, 1990 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)$Id: ftpd.c,v 1.1.1.1 1998/08/21 18:10:32 root Exp $ based on ftpd.c  5.40 (Berkeley) 7/2/91";
#endif /* not lint */

#define SPT_NONE	0	/* don't use it at all */
#define SPT_REUSEARGV	1	/* cover argv with title information */
#define SPT_BUILTIN	2	/* use libc builtin */
#define SPT_PSTAT	3	/* use pstat(PSTAT_SETCMD, ...) */
#define SPT_PSSTRINGS	4	/* use PS_STRINGS->... */
#define SPT_SYSMIPS	5	/* use sysmips() supported by NEWS-OS 6 */
#define SPT_SCO		6	/* write kernel u. area */
#define MAXLINE      2048       /* max line length for setproctitle */
#define SPACELEFT(buf, ptr)  (sizeof buf - ((ptr) - buf))
/* FTP server. */
#include "config.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/wait.h>

#ifdef AIX
#include <sys/id.h>
#include <sys/priv.h>
#endif

#ifdef AUX
#include <compat.h>
#endif

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>

#define FTP_NAMES
#include "ftp.h"
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <setjmp.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#ifdef INTERNAL_LS
#ifdef HAVE_GLOB_H
#include <glob.h>
#else
#include <BeroFTPD_glob.h>
#endif
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif

/*
 *  Arrange to use either varargs or stdargs
 *
 */

#include <stdarg.h>

#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#else
#include <syslog.h>
#endif
#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#include <sys/time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#include "conversions.h"
#include "extensions.h"

#ifdef M_UNIX
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_SYSTEMINFO_H
#include <sys/systeminfo.h>
#endif

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

#ifdef KERBEROS
#include <sys/types.h>
#include <auth.h>
#include <krb.h>
#endif

#ifdef ULTRIX_AUTH
#include <auth.h>
#include <sys/svcinfo.h>
#endif

#ifdef AFS
#include <afs/stds.h>
#include <afs/kautils.h>
int check_afs_password(char *user, char *passwd);
#endif

#ifndef HAVE_LSTAT
#define lstat stat
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#include "pathnames.h"

#ifdef HAVE_GETRLIMIT
#include <sys/resource.h>
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64  /* may be too big */
#endif

#ifndef TRUE
#define  TRUE   1
#endif

#ifndef FALSE
#define  FALSE  !TRUE
#endif

#if !defined(SIGURG)
#define SIGURG	SIGUSR1
#endif

#ifdef MAIL_ADMIN
#include "tool.h"
#include "socket.h"
#define MAILSERVERS 10
#define INCMAILS 10
int mailservers = 0;
char *mailserver[MAILSERVERS];
int incmails = 0;
char *incmail[INCMAILS];
char *mailfrom;
#endif

#ifdef KRB5
#include <krb5.h>
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_generic.h>
gss_ctx_id_t gcontext;
gss_buffer_desc client_name;
int gss_ok;     /* GSSAPI authentication and userok authorization succeeded */
char* gss_services[] = { "ftp", "host", 0 };
char *auth_type;        /* Authentication succeeded?  If so, what type? */
static char *temp_auth_type;
int prot_level;

int auth_data(char *data);
int auth(char *type);
int setlevel(int new_level);
char *radix_error(int e);
int radix_encode(unsigned char inbuf[], unsigned char outbuf[], 
	int *len, int decode);
int reply_gss_error(int code, OM_uint32 maj_stat, OM_uint32 min_stat, char *s);
int check_krb5_password(const char *, const char *);
static int verify_krb_v5_tgt(krb5_ccache);
krb5_error_code krb5_prompter_ftpd(krb5_context, void *,
                                  const char *, int,
                                  krb5_prompt[]);
#endif /* KRB5 */

/* File containing login names NOT to be used on this machine. Commonly used
 * to disallow uucp. */
extern int errno;
extern int pidfd;

extern char *ctime(const time_t *);
#ifndef NO_CRYPT_PROTO
 extern char *crypt(const char *, const char *);
#endif
extern FILE *ftpd_popen(char *program, char *type, int closestderr),
 *fopen(const char *, const char *),
 *freopen(const char *, const char *, FILE *);
extern int ftpd_pclose(FILE *iop), fclose(FILE *);
extern char *wu_getline(),
 *wu_realpath(const char *pathname, char *result, char* chroot_path),
 *fb_realpath(const char *pathname, char *result);
extern void checkports();
extern int routevector();
extern char version[];
extern char *home;              /* pointer to home directory for glob */
extern char cbuf[];
extern off_t restart_point;
extern int yyerrorcalled;

struct sockaddr_in ctrl_addr;
struct sockaddr_in data_source;
struct sockaddr_in data_dest;
struct sockaddr_in his_addr;
struct sockaddr_in pasv_addr;
struct sockaddr_in vect_addr;
int route_vectored=0;
int passive_port_min = -1;
int passive_port_max = -1;

#ifdef VIRTUAL
#ifdef OLDVIRT
char virtual_root[MAXPATHLEN];
char virtual_banner[MAXPATHLEN];
char virtual_email[MAXPATHLEN];
#endif
char hostaddress[32];
 
extern int virtual_mode;
extern int virtual_ftpaccess;
extern int virtual_len;
 
extern struct sockaddr_in virtual_addr;
extern struct sockaddr_in *virtual_ptr;
#ifdef OLDVIRT
char virtual_hostname[MAXHOSTNAMELEN];
char virtual_address[MAXHOSTNAMELEN];
extern int hostmatch();
#endif
#endif

int regexmatch(char *name, char *rgexp);

#ifdef QUOTA
 extern struct dqblk quota;
#endif

int data;
jmp_buf errcatch,
  urgcatch;
int logged_in = 0;
struct passwd *pw;
char chroot_path[MAXPATHLEN];
int debug;
int timeout = 900;              /* timeout after 15 minutes of inactivity */
int maxtimeout = 7200;          /* don't allow idle time to be set beyond 2
                                 * hours */

/* previously defaulted to 1, and -l or -L set them to 1, so that there was
   no way to turn them *off*!  Changed so that the manpage reflects common
   sense.  -L is way noisy; -l we'll change to be "just right".  _H*/
int logging = 0;
int log_commands = 0;
int log_security = 0;
int syslogmsg = 0;
static int wtmp_logging = 1;

#ifdef SECUREOSF
#define SecureWare /* Does this mean it works for all SecureWare? */
#include <prot.h>
#endif

#ifdef HPUX_10_TRUSTED
#include <hpsecurity.h>
#include <prot.h>
#endif

int anonymous = 1;
int guest;
int type;
int form;
int stru;                       /* avoid C keyword */
int mode;
int usedefault = 1;             /* for data transfers */
int pdata = -1;                 /* for passive mode */
int transflag;
int ftwflag;
off_t file_size;
off_t byte_count;
int TCPwindowsize = 0;         /* 0 = use system default */

#ifdef TRANSFER_COUNT
int data_count_total = 0; /* total number of data bytes */
int data_count_in = 0;
int data_count_out = 0;
int byte_count_total = 0; /* total number of general traffic */
int byte_count_in = 0;
int byte_count_out = 0;
int file_count_total = 0; /* total number of data files */
int file_count_in = 0;
int file_count_out = 0;
int xfer_count_total = 0; /* total number of transfers */
int xfer_count_in = 0;
int xfer_count_out = 0;
#ifdef TRANSFER_LIMIT
int file_limit_raw_in = 0;
int file_limit_raw_out = 0;
int file_limit_raw_total = 0;
int file_limit_data_in = 0;
int file_limit_data_out = 0;
int file_limit_data_total = 0;
int data_limit_raw_in = 0;
int data_limit_raw_out = 0;
int data_limit_raw_total = 0;
int data_limit_data_in = 0;
int data_limit_data_out = 0;
int data_limit_data_total = 0;
#endif
#endif

int retrieve_is_data = 1; /* !0=data, 0=general traffic -- for 'ls' */
char LastFileTransferred [MAXPATHLEN] = "";

static char *RootDirectory = NULL;

#if !defined(CMASK) || CMASK == 0
#undef CMASK
#define CMASK 022
#endif
mode_t defumask = CMASK;           /* default umask value */
char defhome [] = "/";
char tmpline[7];
char hostname[MAXHOSTNAMELEN];
char remotehost[MAXHOSTNAMELEN];
char remoteaddr[MAXHOSTNAMELEN];
char *remoteident = "[nowhere yet]";

/* log failures 	27-apr-93 ehk/bm */
#ifdef LOG_FAILED
#define MAXUSERNAMELEN	32
char the_user[MAXUSERNAMELEN];
#endif

/* Access control and logging passwords */
/* ON by default.  */
int use_accessfile = 1;
char guestpw[MAXHOSTNAMELEN];
char privatepw[MAXHOSTNAMELEN];
int nameserved = 0;
extern char authuser[];
extern int authenticated;

/* File transfer logging */
int xferlog = 0;
int log_outbound_xfers = 0;
int log_incoming_xfers = 0;
char logfile[MAXPATHLEN];

/* Allow use of lreply(); this is here since some older FTP clients don't
 * support continuation messages.  In violation of the RFCs... */
int dolreplies = 1;

/* Spontaneous reply text.  To be sent along with next reply to user */
char *autospout = NULL;
int autospout_free = 0;

/* allowed on-the-fly file manipulations (compress, tar) */
int mangleopts = 0;

/* number of login failures before attempts are logged and FTP *EXITS* */
int lgi_failure_threshold = 5;

/* Timeout intervals for retrying connections to hosts that don't accept PORT
 * cmds.  This is a kludge, but given the problems with TCP... */
#define SWAITMAX    90          /* wait at most 90 seconds */
#define SWAITINT    5           /* interval between retries */

int swaitmax = SWAITMAX;
int swaitint = SWAITINT;

void lostconn(int sig);
void randomsig(int sig);
void myoob(int sig);
FILE *getdatasock(char *mode),
 *dataconn(char *name, off_t size, char *mode);
void setproctitle(const char *fmt, ...);
void reply(int, char *fmt, ...);
void lreply(int, char *fmt, ...);

#ifdef NEED_SIGFIX
extern sigset_t block_sigmask;  /* defined in sigfix.c */
#endif

char **Argv = NULL;             /* pointer to argument vector */
char *LastArgv = NULL;          /* end of argv */
char proctitle[BUFSIZ];         /* initial part of title */

#if defined(SKEY) && defined(OPIE)
#error YOU SHOULD NOT HAVE BOTH SKEY AND OPIE DEFINED!!!!!
#endif

#ifdef SKEY
#include <skey.h>
int	pwok = 0;
struct skey skey;
#endif

#ifdef OPIE
#include <opie.h>
int	pwok = 0;
struct opie opiestate;
#endif

#ifdef KERBEROS
void init_krb();
void end_krb();
char krb_ticket_name[100];
#endif /* KERBEROS */

#ifdef ULTRIX_AUTH
int ultrix_check_pass(char *passwd, char *xpasswd);
#endif

#ifdef USE_PAM
static int pam_check_pass(char *user, char *passwd);
#endif

#ifndef INTERNAL_LS
/* ls program commands and options for lreplies on and off */
char  ls_long[1024];
char  ls_short[1024];
char  ls_plain[1024];
#endif
struct aclmember *entry = NULL;

#ifdef DAEMON
int be_daemon = 0;               /* Run standalone? */
int daemon_port = 0;
void do_daemon(int argc, char **argv, char **envp);
#endif
int Bypass_PID_Files = 0;

#ifdef OTHER_PASSWD
#include "getpwnam.h"
char _path_passwd[MAXPATHLEN];
#ifdef SHADOW_PASSWORD
char _path_shadow[MAXPATHLEN];
#endif
#endif

void setup_paths();
void end_login();
#ifdef THROUGHPUT
int send_data(char *, FILE *, FILE *, off_t);
#else
int send_data(FILE *, FILE *, off_t);
#endif
void dolog(struct sockaddr_in *);
void dologout(int);
void perror_reply(int, char *);
int denieduid(uid_t);
int alloweduid(uid_t);
int deniedgid(gid_t);
int allowedgid(gid_t);

#ifdef THROUGHPUT
extern void throughput_calc(char *, int *, double *);
extern void throughput_adjust(char *);
#endif

#ifdef RATIO /* 1998/08/04 K.Wakui */
#define	TRUNC_KB(n)   ((n)/1024+(((n)%1024)?1:0))
/* limit the total time a session */
/* limit the bytes a user can transfer at one sitting */
time_t	login_time;
time_t	limit_time = 0;
size_t	limit_download = 0;
size_t	limit_upload = 0;
size_t	total_download = 0;
size_t	total_free_dl = 0;
size_t	total_upload = 0;
int	upload_download_rate = -1;
int	freefile;
int	is_downloadfree( char * );
#endif /* RATIO */
int	show_message_everytime = 0;
int	show_readme_everytime = 0;


int main(int argc, char **argv, char **envp)
{
    size_t addrlen;
    int  on = 1;
#ifdef IPTOS_LOWDELAY
    int tos;
#endif
    int c;
    int which;
    extern int optopt;
    extern char *optarg;
    struct hostent *shp;
    struct servent *serv;

#if defined(HAVE_SETCOMPAT) && defined(COMPAT_POSIX) && defined(COMPAT_BSDSETUGID)
    setcompat(COMPAT_POSIX | COMPAT_BSDSETUGID);
#endif

    closelog ();
#ifdef FACILITY
    openlog("ftpd", LOG_PID | LOG_NDELAY, FACILITY);
#else
    openlog("ftpd", LOG_PID);
#endif

#ifdef SecureWare
    setluid(1);                         /* make sure there is a valid luid */
    set_auth_parameters(argc,argv);
    setreuid(0, 0);
#endif
#if defined(M_UNIX) && !defined(_M_UNIX)
    res_init();                         /* bug in old (1.1.1) resolver     */
    _res.retrans = 20;                  /* because of fake syslog in 3.2.2 */
    setlogmask(LOG_UPTO(LOG_INFO));
#endif

#ifndef DAEMON
    addrlen = sizeof(his_addr);
    if (getpeername(0, (struct sockaddr *) &his_addr, &addrlen) < 0) {
        syslog(LOG_ERR, "getpeername (%s): %m", argv[0]);
#ifndef DEBUG
        exit(1);
#endif
    }
    addrlen = sizeof(ctrl_addr);
    if (getsockname(0, (struct sockaddr *) &ctrl_addr, &addrlen) < 0) {
        syslog(LOG_ERR, "getsockname (%s): %m", argv[0]);
#ifndef DEBUG
        exit(1);
#endif
    }
#ifdef IPTOS_LOWDELAY
    tos = IPTOS_LOWDELAY;
    if (setsockopt(0, IPPROTO_IP, IP_TOS, (char *) &tos, sizeof(int)) < 0)
          syslog(LOG_WARNING, "setsockopt (IP_TOS): %m");
#endif

    serv = getservbyname ("ftp-data", "tcp");
    if (serv != NULL)
        data_source.sin_port = serv->s_port;
    else
        data_source.sin_port = htons(ntohs(ctrl_addr.sin_port) - 1);
    debug = 0;
#endif /* DAEMON */

    /* Save start and extent of argv for setproctitle. */
    Argv = argv;
    while (*envp)
        envp++;
    LastArgv = envp[-1] + strlen(envp[-1]);

#ifndef DAEMON
    while ((c = getopt(argc, argv, ":aAvdlLioP:qQr:t:T:u:wWX")) != -1) {
#else
    while ((c = getopt(argc, argv, ":aAvdlLiop:P:qQr:sSt:T:u:wWX")) != -1) {
#endif
        switch (c) {

        case 'a':
            use_accessfile = 1;
            break;

        case 'A':
            use_accessfile = 0;
            break;

        case 'v':
            debug = 1;
            break;

        case 'd':
            debug = 1;
            break;

        case 'l':
            logging = 1;
            break;

        case 'L':
            log_commands = 1;
            break;

        case 'i':
            log_incoming_xfers = 1;
            break;

        case 'o':
            log_outbound_xfers = 1;
            break;

        case 'q':
            Bypass_PID_Files = 0;
            break;
            
        case 'Q':
            Bypass_PID_Files = 1;
            break;
            
        case 'r':
          if ((optarg != NULL) && (optarg[0] != '\0')) {
             RootDirectory = malloc (strlen (optarg) + 1);
             if (RootDirectory != NULL)
                 strcpy (RootDirectory, optarg);
         }
         break;

        case 'P':
            data_source.sin_port = htons(atoi(optarg));
            break;
            
#ifdef DAEMON
	case 'p':
	    daemon_port = atoi(optarg);
	    break;
	    
        case 's':
            be_daemon = 1;
            break;

        case 'S':
            be_daemon = 2;
            break;
#endif /* DAEMON */

        case 't':
            timeout = atoi(optarg);
            if (maxtimeout < timeout)
                maxtimeout = timeout;
            break;

        case 'T':
            maxtimeout = atoi(optarg);
            if (timeout > maxtimeout)
                timeout = maxtimeout;
            break;

        case 'u':
             {
             unsigned int val = 0;

             while (*optarg && *optarg >= '0' && *optarg <= '9')
                 val = val * 8 + *optarg++ - '0';
             if (*optarg || val > 0777)
                 syslog(LOG_ERR, "bad value for -u");
             else
                 defumask = val;
             }
             break;

        case 'w':
            wtmp_logging = 1;
            break;

        case 'W':
            wtmp_logging = 0;
            break;

        case 'X':
            syslogmsg = 1;
            break;

        case ':':
            syslog(LOG_ERR, "option -%c requires an argument", optopt);
            break;

        default:
            syslog(LOG_ERR, "unknown option -%c ignored", optopt);
            break;
        }
    }
    freopen(_PATH_DEVNULL, "w", stderr);

    /* Checking for random signals ... */
#ifdef NEED_SIGFIX
    sigemptyset(&block_sigmask);
#endif
#ifndef SIG_DEBUG
#ifdef SIGHUP
    signal(SIGHUP, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGHUP);
#endif
#endif
#ifdef SIGINT
    signal(SIGINT, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGINT);
#endif
#endif
#ifdef SIGQUIT
    signal(SIGQUIT, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGQUIT);
#endif
#endif
#ifdef SIGILL
    signal(SIGILL, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGILL);
#endif
#endif
#ifdef SIGTRAP
    signal(SIGTRAP, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGTRAP);
#endif
#endif
#ifdef SIGIOT
    signal(SIGIOT, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGIOT);
#endif
#endif
#ifdef SIGEMT
    signal(SIGEMT, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGEMT);
#endif
#endif
#ifdef SIGFPE
    signal(SIGFPE, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGFPE);
#endif
#endif
#ifdef SIGKILL
    signal(SIGKILL, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGKILL);
#endif
#endif
#ifdef SIGBUS
    signal(SIGBUS, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGBUS);
#endif
#endif
#ifdef SIGSEGV
    signal(SIGSEGV, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGSEGV);
#endif
#endif
#ifdef SIGSYS
    signal(SIGSYS, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGSYS);
#endif
#endif
#ifdef SIGALRM
    signal(SIGALRM, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGALRM);
#endif
#endif
#ifdef SIGSTOP
    signal(SIGSTOP, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGSTOP);
#endif
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGTSTP);
#endif
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGTTIN);
#endif
#endif
#ifdef SIGTTOU
    signal(SIGTTOU, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGTTOU);
#endif
#endif
#ifdef SIGIO
    signal(SIGIO, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGIO);
#endif
#endif
#ifdef SIGXCPU
    signal(SIGXCPU, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGXCPU);
#endif
#endif
#ifdef SIGXFSZ
    signal(SIGXFSZ, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGXFSZ);
#endif
#endif
#ifdef SIGWINCH
    signal(SIGWINCH, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGWINCH);
#endif
#endif
#ifdef SIGVTALRM
    signal(SIGVTALRM, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGVTALRM);
#endif
#endif
#ifdef SIGPROF
    signal(SIGPROF, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGPROF);
#endif
#endif
#ifdef SIGUSR1
    signal(SIGUSR1, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGUSR1);
#endif
#endif
#ifdef SIGUSR2
    signal(SIGUSR2, randomsig);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGUSR2);
#endif
#endif

#ifdef SIGPIPE
    signal(SIGPIPE, lostconn);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGPIPE);
#endif
#endif
#ifdef SIGCHLD
    signal(SIGCHLD, SIG_IGN);
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGCHLD);
#endif
#endif

#ifdef SIGURG
    if ((int) signal(SIGURG, myoob) < 0)
        syslog(LOG_ERR, "signal: %m");
#ifdef NEED_SIGFIX
    sigaddset(&block_sigmask, SIGURG);
#endif
#endif
#endif /* SIG_DEBUG */

#ifdef DAEMON
    if (be_daemon != 0)
        do_daemon(argc, argv, envp);
    addrlen = sizeof(his_addr);
    if (getpeername(0, (struct sockaddr *) &his_addr, &addrlen) < 0) {
        syslog(LOG_ERR, "getpeername (%s): %m", argv[0]);
#ifndef DEBUG
        exit(1);
#endif
    }
    addrlen = sizeof(ctrl_addr);
    if (getsockname(0, (struct sockaddr *) &ctrl_addr, &addrlen) < 0) {
        syslog(LOG_ERR, "getsockname (%s): %m", argv[0]);
#ifndef DEBUG
        exit(1);
#endif
    }
#ifdef IPTOS_LOWDELAY
    tos = IPTOS_LOWDELAY;
    if (setsockopt(0, IPPROTO_IP, IP_TOS, (char *) &tos, sizeof(int)) < 0)
          syslog(LOG_WARNING, "setsockopt (IP_TOS): %m");
#endif

    data_source.sin_port = htons(ntohs(ctrl_addr.sin_port) - 1);
    debug = 0;
#endif /* DAEMON */

    /* Try to handle urgent data inline */
#ifdef SO_OOBINLINE
    if (setsockopt(0, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(int)) < 0)
        syslog(LOG_ERR, "setsockopt (SO_OOBINLINE): %m");
#endif

#ifdef  F_SETOWN
    if (fcntl(fileno(stdin), F_SETOWN, getpid()) == -1)
        syslog(LOG_ERR, "fcntl F_SETOWN: %m");
#elif defined(SIOCSPGRP)
    {
        int pid;
        pid = getpid();
        if (ioctl(fileno(stdin), SIOCSPGRP, &pid) == -1)
            syslog(LOG_ERR, "ioctl SIOCSPGRP: %m");
    }
#endif

    if ( RootDirectory != NULL ) {
      if ((chroot (RootDirectory) < 0)
      ||  (chdir ("/") < 0)) {
        syslog (LOG_ERR, "Cannot chroot to initial directory, aborting.");
        exit (1);
      }
    }

    dolog(&his_addr);
    /* Set up default state */
    data = -1;
    type = TYPE_A;
    form = FORM_N;
    stru = STRU_F;
    mode = MODE_S;
    tmpline[0] = '\0';
    yyerrorcalled = 0;
#ifdef KRB5
    prot_level = PROT_C;
#endif

#ifdef OTHER_PASSWD
    strcpy(_path_passwd, "/etc/passwd");
#ifdef SHADOW_PASSWORD
    strcpy(_path_shadow, "/etc/shadow");
#endif
#endif
    
    setup_paths();
    access_init();
    if ((getaclentry("hostname", &entry)) && ARG0) {
        strncpy(hostname, ARG0, sizeof(hostname));
        hostname[sizeof(hostname)-1]='\0';
    } else {
#ifdef HAVE_SYS_SYSTEMINFO_H
        sysinfo(SI_HOSTNAME, hostname, sizeof (hostname));
#else
        gethostname(hostname, sizeof (hostname));
#endif
    /* set the FQDN here */
        shp = gethostbyname(hostname);
        if (shp != NULL) {
          strncpy(hostname, shp->h_name, sizeof(hostname));
          hostname[sizeof(hostname)-1]='\0';
        }
    }
#if defined(VIRTUAL) && defined(OLDVIRT)
    virtual_root[0] = '\0';
    virtual_banner[0] = '\0';
#endif
    route_vectored=routevector();
    authenticate();
    conv_init();
    /* Create a composite source identification string, to improve the logging
     * when RFC 931 is being used. */
    {
        int n = 20+strlen(remotehost)+strlen(remoteaddr)+
            (authenticated ? strlen(authuser+5) : 0);
        if ((remoteident = malloc(n)) == NULL) {
            syslog(LOG_ERR, "malloc (%s): %m", argv[0]);
#ifndef DEBUG
            exit(1);
#endif
        } else if (authenticated)
            sprintf(remoteident,"%s @ %s [%s]",
                authuser, remotehost, remoteaddr);
        else
            sprintf(remoteident,"%s [%s]", remotehost, remoteaddr);
    }
     
#if defined(VIRTUAL) && defined(OLDVIRT)
    /*
    ** If virtual_mode is set at this point then an alternate ftpaccess
    ** is in use.  Otherwise we need to check the Master ftpaccess file
    ** to see if the site is only using the "virtual" directives to
    ** specify virtual site directives.
    **
    ** In this manner an admin can put a virtual site in the ftpservers
    ** file if they need expanded configuration support or can use the
    ** minimal root/banner/logfile if they do not need any more than that.
    */

    if (virtual_mode) {
        /* Get the root of the virtual server directory */
        entry = (struct aclmember *) NULL;
        if (getaclentry("root", &entry)) {
            if (ARG0)
                strcpy(virtual_root, ARG0);
        }
        
        /* Get the logfile to use */
        entry = (struct aclmember *) NULL;
        if (getaclentry("logfile", &entry)) {
            if (ARG0)
                strcpy(logfile, ARG0);
        }
    } else {
        virtual_hostname[0] = '\0';
        virtual_address[0] = '\0';
        virtual_len = sizeof(virtual_addr);
        if (getsockname(0, (struct sockaddr *) &virtual_addr, &virtual_len) == 0) {
            virtual_ptr = (struct sockaddr_in *) &virtual_addr;
            strcpy (virtual_address, inet_ntoa(virtual_ptr->sin_addr));
            shp = gethostbyaddr((char *) &virtual_ptr->sin_addr, sizeof (struct in_addr), AF_INET);
            if (shp != NULL) {
                strncpy(virtual_hostname, shp->h_name, sizeof(virtual_hostname));
                virtual_hostname[sizeof(virtual_hostname)-1]='\0';
            }
            entry = (struct aclmember *) NULL;
            while (getaclentry("virtual", &entry)) {
                if (!ARG0 || !ARG1 || !ARG2)
                    continue;
                if (hostmatch(ARG0, virtual_address, virtual_hostname)) {
                    if(!strcasecmp(ARG1, "root")) {
                        syslog(LOG_NOTICE, "VirtualFTP Connect to: %s [%s]",
                               virtual_hostname, virtual_address);
                        virtual_mode = 1;
                        strncpy(virtual_root, ARG2, MAXPATHLEN);
                        virtual_root[MAXPATHLEN-1]='\0';
                        /* reset hostname to this virtual name */
                        strcpy(hostname, virtual_hostname);
                        virtual_email[0] = '\0';
                    }
                    else if(!strcasecmp(ARG1, "email"))
                        strncpy(virtual_email, ARG2, MAXPATHLEN);
                    else if(!strcasecmp(ARG1, "banner"))
                        strncpy(virtual_banner, ARG2, MAXPATHLEN);
                    else if(!strcasecmp(ARG1, "logfile"))
                        strncpy(logfile, ARG2, MAXPATHLEN);
                    else if(!strcasecmp(ARG1, "hostname")) {
                        strncpy(hostname, ARG2, sizeof(hostname));
                        hostname[sizeof(hostname)-1]='\0';
                    } else if(!strcasecmp(ARG1, "email")) {
		        strncpy(virtual_email, ARG2, sizeof (virtual_email));
		        virtual_email[sizeof(virtual_email)-1]='\0';
		    }
                }
            }
        }
    }
 
#ifdef VIRTUAL_DEBUG
    lreply(220, "_path_ftpaccess == %s", _path_ftpaccess);
    lreply(220, "_path_ftpusers == %s", _path_ftpusers);
    lreply(220, "_path_ftphosts == %s", _path_ftphosts);
    lreply(220, "_path_private == %s", _path_private);
    lreply(220, "_path_cvt == %s", _path_cvt);
    if (virtual_mode) {
       if (virtual_ftpaccess)
           lreply(220, "VIRTUAL Mode: Using %s specific %s access file",
                   hostname, _path_ftpaccess);
       else
           lreply(220, "VIRTUAL Mode: Using Master access file %s",
                   _path_ftpaccess);
 
       lreply(220, "virtual_root == %s", virtual_root);
       if (!virtual_ftpaccess)
           lreply(220, "virtual_banner == %s", virtual_banner);
    }
    lreply(220, "logfile == %s", logfile);
#endif
 
#elif defined(VIRTUAL)
    entry = (struct aclmember *) NULL;
    if(getaclentry("virtual", &entry))
        syslog(LOG_ERR,"ERROR: virtual command used in /etc/ftpaccess without supporting it.");
#endif /* VIRTUAL && OLDVIRT */
#ifdef MAIL_ADMIN
    mailservers=0;
    incmails=0;
    entry=(struct aclmember *) NULL;
    while (getaclentry("mailserver", &entry) && ARG0 && mailservers<MAILSERVERS)
        mailserver[mailservers++] = strdup(ARG0);
    if (mailservers==0)
        mailserver[mailservers++] = strdup("localhost");
    while (getaclentry("incmail", &entry) && ARG0 && incmails<INCMAILS)
        incmail[incmails++]=strdup(ARG0);
    if(getaclentry("mailfrom", &entry) && ARG0)
        mailfrom=strdup(ARG0);
    else
        mailfrom=strdup("BeroFTPD");
#endif
        
    if (is_shutdown(1, 1) != 0) {
        syslog(LOG_INFO, "connection refused (server shut down) from %s",
               remoteident);
        reply(500, "%s FTP server shut down -- please try again later.",
              hostname);
        exit(0);
    }

    show_banner(220);

#ifndef INTERNAL_LS
    entry = (struct aclmember *) NULL;
    if (getaclentry("lslong", &entry) && ARG0 && (int)strlen(ARG0) > 0) {
          strcpy(ls_long,ARG0);
          for (which = 1; (which < MAXARGS) && ARG[which]; which++) {
             strcat(ls_long," ");
             strcat(ls_long,ARG[which]);
          }
    } else {
#if defined(SVR4) || defined(ISC) || defined(sinix)
#if defined(AIX) || defined(SOLARIS2)
          strcpy(ls_long,"/bin/ls -lA");
#else
          strcpy(ls_long,"/bin/ls -la");
#endif
#else
          strcpy(ls_long,"/bin/ls -lgA");
#endif
    }
    strcat(ls_long," %s");

    entry = (struct aclmember *) NULL;
    if (getaclentry("lsshort", &entry) && ARG0 && (int)strlen(ARG0) > 0) {
          strcpy(ls_short,ARG0);
          for (which = 1; (which < MAXARGS) && ARG[which]; which++) {
             strcat(ls_short," ");
             strcat(ls_short,ARG[which]);
      }
    } else {
#if defined(SVR4) || defined(ISC) || defined(sinix)
#if defined(AIX) || defined(SOLARIS2)
          strcpy(ls_short,"/bin/ls -lA");
#else
          strcpy(ls_short,"/bin/ls -la");
#endif
#else
          strcpy(ls_short,"/bin/ls -lgA");
#endif
    }
    strcat(ls_short," %s");
    entry = (struct aclmember *) NULL;
    if (getaclentry("lsplain", &entry) && ARG0 && (int)strlen(ARG0) > 0) {
        strcpy(ls_plain,ARG0);
        for (which = 1; (which < MAXARGS) && ARG[which]; which++) {
             strcat(ls_plain," ");
             strcat(ls_plain,ARG[which]);
        }
    } else
        strcpy(ls_plain,"/bin/ls");
    strcat(ls_plain," %s");
#endif

    {
    int version_option = 0;
    entry = NULL;
    if (getaclentry("greeting", &entry) && ARG0) {
        if (!strcasecmp(ARG0, "full")) version_option = 0;
	else if (!strcasecmp(ARG0, "terse")) version_option = 2;
	else if (!strcasecmp(ARG0, "brief")) version_option = 1;
	else if (!strcasecmp(ARG0, "nover")) version_option = 3;
    }
    switch(version_option) {
    default:
        reply(220, "%s FTP server (%s) ready.", hostname, version);
        break;
    case 1:
        reply(220, "%s FTP server ready.", hostname);
        break;
    case 2:
        reply(220, "FTP server ready.");
        break;
    case 3:
        reply(220, "%s FTP server (BeroFTPD) ready.", hostname);
        break;
    }
    }

    setjmp(errcatch);

    while (1)
        yyparse();
    /* NOTREACHED */
}

void randomsig(int sig)
{
#ifdef HAVE_SIGLIST
    syslog(LOG_ERR, "exiting on signal %d: %s", sig, sys_siglist[sig] );
#else
    syslog(LOG_ERR, "exiting on signal %d", sig);
#endif
    chdir("/");
    signal(SIGIOT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    exit (1);
    /* dologout(-1); *//* NOTREACHED */
}

void lostconn(int sig)
{
    if (debug)
        syslog(LOG_DEBUG, "lost connection to %s", remoteident);
    dologout(-1);
}

static char ttyline[20];

#ifdef MAPPING_CHDIR
/* Keep track of the path the user has chdir'd into and respond with
 * that to pwd commands.  This is to avoid having the absolue disk
 * path returned, which I want to avoid.
 */
char mapped_path[ MAXPATHLEN ] = "/";

char * mapping_getwd(char *path)
{
      strcpy( path, mapped_path );
      return path;
}

/* Make these globals rather than local to mapping_chdir to avoid stack overflow */
char pathspace[ MAXPATHLEN ];
char old_mapped_path[ MAXPATHLEN ];

void do_elem(char *dir)
{
      /* . */
      if( dir[0] == '.' && dir[1] == '\0' ){
              /* ignore it */
              return;
      }

      /* .. */
      if( dir[0] == '.' && dir[1] == '.' && dir[2] == '\0' ){
              char *last;
              /* lop the last directory off the path */
              if( last = strrchr( mapped_path, '/') ){
                      /* If start of pathname leave the / */
                      if( last == mapped_path )
                              last++;
                      *last = '\0';
              }
              return;
      }

      /* append the dir part with a leading / unless at root */
      if( !(mapped_path[0] == '/' && mapped_path[1] == '\0') )
              strcat( mapped_path, "/" );
      strcat( mapped_path, dir );
}

int mapping_chdir(char *orig_path)
{
      int ret;
      char *sl, *path;

      strcpy( old_mapped_path, mapped_path );
      path = &pathspace[0];
      strcpy( path, orig_path );

      /* / at start of path, set the start of the mapped_path to / */
      if( path[0] == '/' ){
              mapped_path[0] = '/';
              mapped_path[1] = '\0';
              path++;
      }

      while( (sl = strchr( path, '/' )) ){
              char *dir, *last;
              dir = path;
              *sl = '\0';
              path = sl + 1;
              if( *dir )
                      do_elem( dir );
              if( *path == '\0' )
                      break;
      }
      if( *path )
              do_elem( path );

      if( (ret = chdir( mapped_path )) < 0 ){
              strcpy( mapped_path, old_mapped_path );
      }

      return ret;
}
/* From now on use the mapping version */

#define chdir(d) mapping_chdir(d)
#define getwd(d) mapping_getwd(d)
#define getcwd(d,u) mapping_getwd(d)

#endif /* MAPPING_CHDIR */

/* Helper function for sgetpwnam(). */
char * sgetsave(char *s)
{
    char *new;
    
    new = (char *) malloc(strlen(s) + 1);

    if (new == NULL) {
        perror_reply(421, "Local resource failure: malloc");
        dologout(1);
        /* NOTREACHED */
    }
    strcpy(new, s);
    return (new);
}

/* Save the result of a getpwnam.  Used for USER command, since the data
 * returned must not be clobbered by any other command (e.g., globbing). */
struct passwd * sgetpwnam(char *name)
{
    static struct passwd save;
    register struct passwd *p;
#ifdef M_UNIX
    struct passwd *ret = (struct passwd *) NULL;
#endif
    char *sgetsave(char *s);
#ifdef KERBEROS
    register struct authorization *q;
#endif /* KERBEROS */

#if defined(SecureWare) || defined(HPUX_10_TRUSTED)
    struct pr_passwd *pr;
#endif

#ifdef KERBEROS
    init_krb();
    q = getauthuid(p->pw_uid);
    end_krb();
#endif /* KERBEROS */

#ifdef M_UNIX
# if defined(SecureWare) || defined(HPUX_10_TRUSTED)
    if ((pr = getprpwnam(name)) == NULL)
        goto DONE;
# endif /* SecureWare || HPUX_10_TRUSTED */
#ifdef OTHER_PASSWD
    if ((p = bero_getpwnam(name,_path_passwd)) == NULL)
#else
    if ((p = getpwnam(name)) == NULL)
#endif
        goto DONE;
#else   /* M_UNIX */
# if defined(SecureWare) || defined(HPUX_10_TRUSTED)
    if ((pr = getprpwnam(name)) == NULL)
        return((struct passwd *) pr);
# endif /* SecureWare || HPUX_10_TRUSTED */
#ifdef OTHER_PASSWD
    if ((p = bero_getpwnam(name, _path_passwd)) == NULL)
#else
    if ((p = getpwnam(name)) == NULL)
#endif
        return (p);
#endif  /* M_UNIX */

    if (save.pw_name)   free(save.pw_name);
    if (save.pw_gecos)  free(save.pw_gecos);
    if (save.pw_dir)    free(save.pw_dir);
    if (save.pw_shell)  free(save.pw_shell);
    if (save.pw_passwd) free(save.pw_passwd);

    save = *p;

    save.pw_name = sgetsave(p->pw_name);

#ifdef KERBEROS
    save.pw_passwd = sgetsave(q->a_password);
#elif defined(SecureWare) || defined(HPUX_10_TRUSTED)
    if (pr->uflg.fg_encrypt && pr->ufld.fd_encrypt && *pr->ufld.fd_encrypt)
       save.pw_passwd = sgetsave(pr->ufld.fd_encrypt);
    else
       save.pw_passwd = sgetsave("");
#else
    save.pw_passwd = sgetsave(p->pw_passwd);
#endif
#ifdef SHADOW_PASSWORD
        if (p) {
           struct spwd *spw;
#ifdef OTHER_PASSWD
           if ((spw = bero_getspnam(p->pw_name, _path_shadow)) != NULL) {
#else
	   setspent();
           if ((spw = getspnam(p->pw_name)) != NULL) {
#endif
               int expired = 0;
	       /*XXX Does this work on all Shadow Password Implementations? */
	       /* it is supposed to work on Solaris 2.x*/
               time_t now;
               long today;
               
               now = time((time_t*) 0);
               today = now / (60*60*24);
               
               if ((spw->sp_expire > 0) && (spw->sp_expire < today)) expired++;
               if ((spw->sp_max > 0) && (spw->sp_lstchg > 0) &&
		   (spw->sp_lstchg + spw->sp_max < today)) expired++;
	       free(save.pw_passwd);
               save.pw_passwd = sgetsave(expired?"":spw->sp_pwdp);
           }
/* Don't overwrite the password if the shadow read fails, getpwnam() is NIS
   aware but getspnam() is not. */
/* Shadow passwords are optional on Linux.  --marekm */
#if !defined(LINUX) && !defined(UNIXWARE)
           else{
	     free(save.pw_passwd);
	     save.pw_passwd = sgetsave("");
	   }
#endif
/* marekm's fix for linux proc file system shadow passwd exposure problem */
#ifndef OTHER_PASSWD
	   endspent();		
#endif
        }
#endif
    save.pw_gecos = sgetsave(p->pw_gecos);
    save.pw_dir = sgetsave(p->pw_dir);
    save.pw_shell = sgetsave(p->pw_shell);
#ifdef M_UNIX
    ret = &save;
DONE:
    endpwent();
#endif
#if defined(SecureWare) || defined(HPUX_10_TRUSTED)
    endprpwent();
#endif
#ifdef M_UNIX
    return(ret);
#else
    return(&save);
#endif
}
#if defined(SKEY) && !defined(__NetBSD__)
/*
 * From Wietse Venema, Eindhoven University of Technology. 
 */
/* skey_challenge - additional password prompt stuff */
char   *skey_challenge(char *name, struct passwd *pwd, int pwok)
{
    static char buf[128];
    char sbuf[40];

    /* Display s/key challenge where appropriate. */

    if (pwd == NULL || skeychallenge(&skey, pwd->pw_name, sbuf)) {
	sprintf(buf, "Password required for %s.", name);
		/* Stevenson! - 03/02/1999 */
	skey.n = -1;
		/* end Stevenson! */
    } else {
	sprintf(buf, "%s %s for %s.", sbuf,
		pwok ? "allowed" : "required", name);
    }
    return (buf);
}
#endif
int login_attempts;             /* number of failed login attempts */
int askpasswd;                  /* had user command, ask for passwd */
#ifndef HELP_CRACKERS
int DenyLoginAfterPassword;
char DelayedMessageFile [MAXPATHLEN];
extern void pr_mesg (int msgcode, char *msgfile);
#endif

#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
static int defaultserver_allow (const char *username)
{
    struct aclmember *entry = NULL;
    int which;

    while (getaclentry("defaultserver", &entry))
        if (ARG0 && !strcasecmp(ARG0, "allow"))
            for (which=1; (which < MAXARGS) && ARG[which]; which++)
                if (!strcasecmp (username, ARG[which]) || !strcmp ("*", ARG[which]))
                    return (1);
    return (0);
}

static int defaultserver_deny (const char *username)
{
    struct aclmember *entry = NULL;
    int which;

    while (getaclentry("defaultserver", &entry))
        if (ARG0 && !strcasecmp(ARG0, "deny"))
            for (which=1; (which < MAXARGS) && ARG[which]; which++)
                if (!strcasecmp (username, ARG[which]) || !strcmp ("*", ARG[which]))
                    return (1);
    return (0);
}

static int defaultserver_private (void)
{
    struct aclmember *entry = NULL;

    while (getaclentry("defaultserver", &entry))
        if (ARG0 && !strcasecmp(ARG0, "private"))
            return (1);
    return (0);
}
#endif

/* USER command. Sets global passwd pointer pw if named account exists and is
 * acceptable; sets askpasswd if a PASS command is expected.  If logged in
 * previously, need to reset state.  If name is "ftp" or "anonymous", the
 * name is not in _path_ftpusers, and ftp account exists, set anonymous and
 * pw, then just return.  If account doesn't exist, ask for passwd anyway.
 * Otherwise, check user requesting login privileges.  Disallow anyone who
 * does not have a standard shell as returned by getusershell().  Disallow
 * anyone mentioned in the file _path_ftpusers to allow people such as root
 * and uucp to be avoided. */

void user(char *name)
{
    char *cp;
    char *shell;
    char *getusershell();
#ifdef	BSD_AUTH
    char *auth;
    extern int ext_auth;
    extern char *start_auth();
#endif

    if (logged_in) {
#ifdef VERBOSE_ERROR_LOGING
        syslog(LOG_NOTICE, "FTP LOGIN REFUSED (already logged in as %s) FROM %s, %s",
               pw->pw_name, remoteident, name);
#endif
	reply(530, "Already logged in.");
	return;
    }
#ifndef HELP_CRACKERS
    askpasswd = 1;
    DenyLoginAfterPassword = 0;
    DelayedMessageFile [0] = '\0';
#endif
#ifdef	BSD_AUTH
    if (auth = strchr(name, ':'))
        *auth++ = 0;
#endif

#ifdef HOST_ACCESS                     /* 19-Mar-93    BM              */
    if (!rhost_ok(name, remotehost, remoteaddr))
    {
#ifndef HELP_CRACKERS
        DenyLoginAfterPassword = 1;
        syslog(LOG_NOTICE,
               "FTP LOGIN REFUSED (name in %s) FROM %s, %s",
                _path_ftphosts, remoteident, name);
#else
        reply(530, "User %s access denied.", name);
        syslog(LOG_NOTICE,
               "FTP LOGIN REFUSED (name in %s) FROM %s, %s",
               _path_ftphosts, remoteident, name);
            return;
#endif
    }
#endif

#ifdef LOG_FAILED                       /* 06-Nov-92    EHK             */
    strncpy(the_user, name, MAXUSERNAMELEN - 1);
#endif

    anonymous = 0;
    acl_remove();

    if (!strcasecmp(name, "ftp") || !strcasecmp(name, "anonymous")) {
      struct aclmember *entry = NULL;
      int machineok=1;
      char guestservername[MAXHOSTNAMELEN];
      guestservername[0]='\0';

#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
      if (!virtual_mode && defaultserver_private ()) {
#ifndef HELP_CRACKERS
	  DenyLoginAfterPassword = 1;
          syslog(LOG_NOTICE, "FTP LOGIN REFUSED (anonymous ftp denied on default server) FROM %s, %s",
		 remoteident, name);
#else
          reply(530, "User %s access denied.", name);
          syslog(LOG_NOTICE,
		"FTP LOGIN REFUSED (anonymous ftp denied on default server) FROM %s, %s",
		remoteident, name);
          return;
#endif
      }
#endif
      {
#ifdef OTHER_PASSWD
      struct passwd *pw = bero_getpwnam("ftp", _path_passwd);
#else
      struct passwd *pw = getpwnam("ftp");
#endif

      if (pw
          && ((denieduid(pw->pw_uid) && !alloweduid(pw->pw_uid))
          ||  (deniedgid(pw->pw_gid) && !allowedgid(pw->pw_gid)))) {
#ifndef HELP_CRACKERS
        DenyLoginAfterPassword = 1;
          syslog(LOG_NOTICE, "FTP LOGIN REFUSED (ftp in denied-uid) FROM %s, %s",
                 remoteident, name);
#else
          reply(530, "User %s access denied.", name);
          syslog(LOG_NOTICE,
                 "FTP LOGIN REFUSED (ftp in denied-uid) FROM %s, %s",
                 remoteident, name);
          return;
#endif
      }
      }

      if (checkuser("ftp") || checkuser("anonymous")) {
#ifndef HELP_CRACKERS
          DenyLoginAfterPassword = 1;
          syslog(LOG_NOTICE, "FTP LOGIN REFUSED (ftp in %s) FROM %s, %s",
                 _path_ftpusers, remoteident, name);
#else
          reply(530, "User %s access denied.", name);
          syslog(LOG_NOTICE,
	       "FTP LOGIN REFUSED (ftp in %s) FROM %s, %s",
	       _path_ftpusers, remoteident, name);
          return;
#endif
         
        /*
        ** Algorithm used:
        ** - if no "guestserver" directive is present,
        **     anonymous access is allowed, for backward compatibility.
        ** - if a "guestserver" directive is present,
        **     anonymous access is restricted to the machines listed,
        **     usually the machine whose CNAME on the current domain
        **     is "ftp"...
        **
        ** the format of the "guestserver" line is
        ** guestserver [<machine1> [<machineN>]]
        ** that is, "guestserver" will forbid anonymous access on all machines
        ** while "guestserver ftp inf" will allow anonymous access on
        ** the two machines whose CNAMES are "ftp.enst.fr" and "inf.enst.fr".
        **
        ** if anonymous access is denied on the current machine,
        ** the user will be asked to use the first machine listed (if any)
        ** on the "guestserver" line instead:
        ** 530- Guest login not allowed on this machine,
        **      connect to ftp.enst.fr instead.
        **
        ** -- <Nicolas.Pioch@enst.fr>
        */
      } else if (getaclentry("guestserver", &entry)
                 && ARG0 && (int)strlen(ARG0) > 0) {
        struct hostent *tmphostent;

        /*
        ** if a "guestserver" line is present,
        ** default is not to allow guest logins
        */
        machineok=0;

        if (hostname[0]
            && ((tmphostent=gethostbyname(hostname)))) {

          /*
          ** hostname is the only first part of the FQDN
          ** this may or may not correspond to the h_name value
          ** (machines with more than one IP#, CNAMEs...)
          ** -> need to fix that, calling gethostbyname on hostname
          **
          ** WARNING!
          ** for SunOS 4.x, you need to have a working resolver in the libc
          ** for CNAMES to work properly.
          ** If you don't, add "-lresolv" to the libraries before compiling!
          */
          char dns_localhost[MAXHOSTNAMELEN];
          int machinecount;

          strncpy(dns_localhost,
                  tmphostent->h_name,
                  sizeof(dns_localhost));
          dns_localhost[sizeof(dns_localhost)-1]='\0';

          for (machinecount=0;
               entry->arg[machinecount] && (entry->arg[machinecount])[0];
               machinecount++) {

            if ((tmphostent=gethostbyname(entry->arg[machinecount]))) {
              /*
              ** remember the name of the first machine for redirection
              */

              if ((!machinecount) && tmphostent->h_name) {
                strncpy(guestservername, entry->arg[machinecount],
                        sizeof(guestservername));
                guestservername[sizeof(guestservername)-1]='\0';
              }

              if (!strcasecmp(tmphostent->h_name, dns_localhost)) {
                machineok++;
                break;
              }
            }
          }
        }
      }
      if (!machineok) {
        if (guestservername[0])
          reply(530,
             "Guest login not allowed on this machine, connect to %s instead.",
                guestservername);
        else
          reply(530,
                "Guest login not allowed on this machine.");
        syslog(LOG_NOTICE,
               "FTP LOGIN REFUSED (localhost not in guestservers) FROM %s, %s",
               remoteident, name);
        /* End of the big patch -- Nap */

        } else if ((pw = sgetpwnam("ftp")) != NULL) {
            anonymous = 1;      /* for the access_ok call */
            if (access_ok(530) < 1) {
#ifndef HELP_CRACKERS
                DenyLoginAfterPassword = 1;
                syslog(LOG_NOTICE, "FTP LOGIN REFUSED (access denied) FROM %s, %s",
                       remoteident, name);
                reply(331, "Guest login ok, send your complete e-mail address as password.");
#else
                reply(530, "User %s access denied.", name);
                syslog(LOG_NOTICE,
                       "FTP LOGIN REFUSED (access denied) FROM %s, %s",
                       remoteident, name);
                dologout(0);
#endif
            } else {
                askpasswd = 1;
/* H* fix: obey use_accessfile a little better.  This way, things set on the
   command line [like xferlog stuff] don't get stupidly overridden.
   XXX: all these checks maybe should be in acl.c and access.c */
		if (use_accessfile)
                    acl_setfunctions();
                reply(331, "Guest login ok, send your complete e-mail address as password.");
            }
        } else {
#ifndef HELP_CRACKERS
            DenyLoginAfterPassword = 1;
            reply(331, "Guest login ok, send your complete e-mail address as password.");
            syslog(LOG_NOTICE, "FTP LOGIN REFUSED (ftp not in /etc/passwd) FROM %s, %s",
                   remoteident, name);
#else
            reply(530, "User %s unknown.", name);
            syslog(LOG_NOTICE,
              "FTP LOGIN REFUSED (ftp not in /etc/passwd) FROM %s, %s",
                   remoteident, name);
#endif
        }
        return;
    }
#ifdef ANON_ONLY
/* H* fix: define the above to completely DISABLE logins by real users,
   despite ftpusers, shells, or any of that rot.  You can always hang your
   "real" server off some other port, and access-control it. */

    else {  /* "ftp" or "anon" -- MARK your conditionals, okay?! */
#ifdef HELP_CRACKERS
      DenyLoginAfterPassword = 1;
      syslog (LOG_NOTICE, "FTP LOGIN REFUSED (not anonymous) FROM %s, %s",
	      remoteident, name);
      reply(331, "Password required for %s.", name);
#else
      reply(530, "User %s unknown.", name);
      syslog (LOG_NOTICE,
	"FTP LOGIN REFUSED (not anonymous) FROM %s, %s",
	  remoteident, name);
#endif
      return;
    }
/* fall here if username okay in any case */
#endif /* ANON_ONLY */

#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
      if (!virtual_mode && defaultserver_deny (name) && !defaultserver_allow (name)) {
#ifndef HELP_CRACKERS
	  DenyLoginAfterPassword = 1;
          syslog(LOG_NOTICE, "FTP LOGIN REFUSED (ftp denied on default server) FROM %s, %s",
		 remoteident, name);
#else
          reply(530, "User %s access denied.", name);
          syslog(LOG_NOTICE,
		"FTP LOGIN REFUSED (ftp denied on default server) FROM %s, %s",
		remoteident, name);
          return;
#endif
      }
#endif

    if ((pw = sgetpwnam(name)) != NULL) {

        if ((denieduid(pw->pw_uid) && !alloweduid(pw->pw_uid))
        ||  (deniedgid(pw->pw_gid) && !allowedgid(pw->pw_gid))) {
#ifndef HELP_CRACKERS
	    DenyLoginAfterPassword = 1;
            syslog(LOG_NOTICE, "FTP LOGIN REFUSED (username in denied-uid) FROM %s, %s",
	  	   remoteident, name);
	    reply(331, "Password required for %s.", name);
#else
            reply(530, "User %s access denied.", name);
            syslog(LOG_NOTICE,
	  	"FTP LOGIN REFUSED (username in denied-uid) FROM %s, %s",
	  	remoteident, name);
#endif
            return;
        }

#ifndef USE_PAM /* PAM should be doing these checks, not ftpd */
        if ((shell = pw->pw_shell) == NULL || *shell == 0)
            shell = _PATH_BSHELL;
        while ((cp = getusershell()) != NULL) {
            while(cp[strlen(cp)-1]==' ' || cp[strlen(cp)-1]=='\n')
            	cp[strlen(cp)-1]=0;
            if (strcmp(cp, shell) == 0)
                break;
        }
        endusershell();
        if (cp == NULL || checkuser(name)) {
#ifndef HELP_CRACKERS
            DenyLoginAfterPassword = 1;
            syslog(LOG_NOTICE, "FTP LOGIN REFUSED (bad shell or username in %s) FROM %s, %s",
                   _path_ftpusers, remoteident, name);
          reply(331, "Password required for %s.", name);
#else
            reply(530, "User %s access denied.", name);
/*            if (logging)	-- inconsistent, removed.  _H*/
                syslog(LOG_NOTICE,
                       "FTP LOGIN REFUSED (bad shell or username in %s) FROM %s, %s",
                       _path_ftpusers, remoteident, name);
#endif
            pw = (struct passwd *) NULL;
            return;
        }
#endif /* USE_PAM */
        /* if user is a member of any of the guestgroups, cause a chroot() */
        /* after they log in successfully                                  */
	if (use_accessfile)		/* see above.  _H*/
	{
            guest = acl_guestgroup(pw);
            if(guest && acl_realgroup(pw))
                guest=0;
        }
    }
#ifdef KRB5
    if (auth_type && strcmp(auth_type, "GSSAPI") == 0) {
            char buf[FTP_BUFSIZ];
            gss_ok = ftpd_userok(&client_name, name) == 0;
            snprintf(buf, sizeof(buf),
		"GSSAPI user %s is%s authorized as %s%s",
                    client_name.value,
                    gss_ok ? "" : " not",
                    name, gss_ok ? "" : "; Password required.");
            /* 232 is per draft-8, but why 331 not 53z? */
            reply(gss_ok ? 232 : 331, "%s", buf);
            syslog(gss_ok ? LOG_INFO : LOG_ERR, "%s", buf);
    } else {
#endif
    if (access_ok(530) < 1) {
#ifndef HELP_CRACKERS
        DenyLoginAfterPassword = 1;
        syslog(LOG_NOTICE, "FTP LOGIN REFUSED (access denied) FROM %s, %s",
               remoteident, name);
        reply(331, "Password required for %s.", name);
#else
        reply(530, "User %s access denied.", name);
        syslog(LOG_NOTICE, "FTP LOGIN REFUSED (access denied) FROM %s, %s",
               remoteident, name);
#endif
        return;
    } else
	if (use_accessfile)		/* see above.  _H*/
            acl_setfunctions();

#ifdef	BSD_AUTH
    if ((cp = start_auth(auth, name, pw)) != NULL) {
	char *s;

	for (;;) {
	    s = strsep(&cp, "\n");
	    if (cp == NULL || *cp == '\0')
		break;
	    lreply(331, s);
	}
	reply(331, s);
    } else {
#endif
#ifdef SKEY
#ifndef __NetBSD__
#ifdef SKEY_NAME
    /* this is the old way, but freebsd uses it */
    pwok = skeyaccess(name, "ftp", remotehost, remoteaddr);
#else
    /* this is the new way */
    pwok = skeyaccess(pw, NULL, remotehost, remoteaddr);
#endif
    reply(331, "%s", skey_challenge(name, pw, pwok));
#else
    if (skey_haskey(name) == 0) {
	char *myskey;

	myskey = skey_keyinfo(name);
	reply(331, "Password [%s] required for %s.",
	    myskey ? myskey : "error getting challenge", name);
    } else
	reply(331, "Password required for %s.", name);
#endif
#else
#ifdef OPIE
   {
	char prompt[OPIE_CHALLENGE_MAX + 1];
	opiechallenge(&opiestate, name, prompt);
	reply(331, "%s", prompt);
    }
#else
    reply(331, "Password required for %s.", name);
#endif
#endif
#ifdef	BSD_AUTH
    }
#endif
#ifdef KRB5
    }
#endif

    askpasswd = 1;
    /* Delay before reading passwd after first failed attempt to slow down
     * passwd-guessing programs. */
#if 0 /* GAL -- signals off when this runs, so it won't work. */
    if (login_attempts)
        sleep((unsigned) login_attempts);
#endif
    return;
}

/* Check if a user is in the file _path_ftpusers */

int checkuser(char *name)
{
    register FILE *fd;
    register char *p;
    char line[BUFSIZ];

    if ((fd = fopen(_path_ftpusers, "r")) != NULL) {
        while (fgets(line, sizeof(line), fd) != NULL)
            if ((p = strchr(line, '\n')) != NULL) {
                *p = '\0';
                if (line[0] == '#')
                    continue;
                if (strcasecmp(line, name) == 0) {
                    fclose(fd);
                    return (1);
                }
            }
        fclose(fd);
    }
    return (0);
}

int denieduid(uid_t uid)
{
    struct aclmember *entry = NULL;
    int which;
    char *ptr;
    struct passwd *pw;

    while (getaclentry ("deny-uid", &entry)) {
        for (which = 0; (which < MAXARGS) && ARG[which]; which++) {
            if (!strcmp (ARG[which], "*"))
                return (1);
            if (ARG[which][0] == '%') {
                if ((ptr = strchr (ARG[which]+1, '-')) == NULL) {
                    if ((ptr = strchr (ARG[which]+1, '+')) == NULL) {
                        if (uid == strtoul (ARG[which]+1, NULL, 0))
                            return (1);
                    } else {
                        *ptr++ = '\0';
                        if ((ARG[which][1] == '\0')
                        ||  (uid >= strtoul (ARG[which]+1, NULL, 0))) {
                            *--ptr = '+';
                            return (1);
                        }
                        *--ptr = '+';
                    }
                } else {
                    *ptr++ = '\0';
                    if (((ARG[which][1] == '\0')
                        || (uid >= strtoul (ARG[which]+1, NULL, 0)))
                    &&  ((*ptr == '\0')
                        || (uid <= strtoul (ptr, NULL, 0)))) {
                        *--ptr = '-';
                        return (1);
                    }
                    *--ptr = '-';
                }
            } else {
#ifdef OTHER_PASSWD
                pw = bero_getpwnam (ARG[which], _path_passwd);
#else
		pw = getpwnam (ARG[which]);
#endif
                if (pw && (uid == pw->pw_uid))
                    return (1);
            }
        }
    }
    return (0);
}

int alloweduid(uid_t uid)
{
    struct aclmember *entry = NULL;
    int which;
    char *ptr;
    struct passwd *pw;

    while (getaclentry ("allow-uid", &entry)) {
        for (which = 0; (which < MAXARGS) && ARG[which]; which++) {
            if (!strcmp (ARG[which], "*"))
                return (1);
            if (ARG[which][0] == '%') {
                if ((ptr = strchr (ARG[which]+1, '-')) == NULL) {
                    if ((ptr = strchr (ARG[which]+1, '+')) == NULL ) {
                        if (uid == strtoul (ARG[which]+1, NULL, 0))
                            return (1);
                    } else {
                        *ptr++ = '\0';
                        if ((ARG[which][1] == '\0')
                        ||  (uid >= strtoul (ARG[which]+1, NULL, 0))) {
                            *--ptr = '+';
                            return (1);
                        }
                        *--ptr = '+';
                    }
                } else {
                    *ptr++ = '\0';
                    if (((ARG[which][1] == '\0')
                        || (uid >= strtoul (ARG[which]+1, NULL, 0)))
                    &&  ((*ptr == '\0')
                        || (uid <= strtoul (ptr, NULL, 0)))) {
                        *--ptr = '-';
                        return (1);
                    }
                    *--ptr = '-';
                }
            } else {
#ifdef OTHER_PASSWD
		pw = bero_getpwnam (ARG[which], _path_passwd);
#else
                pw = getpwnam (ARG[which]);
#endif
                if (pw && (uid == pw->pw_uid))
                    return (1);
            }
        }
    }
    return (0);
}

int deniedgid(gid_t gid)
{
    struct aclmember *entry = NULL;
    int which;
    char *ptr;
    struct group * grp;

    while (getaclentry ("deny-gid", &entry)) {
        for (which = 0; (which < MAXARGS) && ARG[which]; which++) {
            if (!strcmp (ARG[which], "*"))
                return (1);
            if (ARG[which][0] == '%') {
                if ((ptr = strchr (ARG[which]+1, '-')) == NULL) {
                    if ((ptr = strchr (ARG[which]+1, '+')) == NULL) {
                        if (gid == strtoul (ARG[which]+1, NULL, 0))
                            return (1);
                    } else {
                        *ptr++ = '\0';
                        if ((ARG[which][1] == '\0')
                        ||  (gid >= strtoul (ARG[which]+1, NULL, 0))) {
                            *--ptr = '+';
                            return (1);
                        }
                        *--ptr = '+';
                    }
                } else {
                    *ptr++ = '\0';
                    if (((ARG[which][1] == '\0')
                        || (gid >= strtoul (ARG[which]+1, NULL, 0)))
                    &&  ((*ptr == '\0')
                        || (gid <= strtoul (ptr, NULL, 0)))) {
                        *--ptr = '-';
                        return (1);
                    }
                    *--ptr = '-';
                }
            } else {
                grp = getgrnam (ARG[which]);
                if (grp && (gid == grp->gr_gid))
                    return (1);
            }
        }
    }
    return (0);
}

int allowedgid(gid_t gid)
{
    struct aclmember *entry = NULL;
    int which;
    char *ptr;
    struct group *grp;

    while (getaclentry ("allow-gid", &entry)) {
        for (which = 0; (which < MAXARGS) && ARG[which]; which++) {
            if (!strcmp (ARG[which], "*"))
                return (1);
            if (ARG[which][0] == '%') {
                if ((ptr = strchr (ARG[which]+1, '-')) == NULL) {
                    if ((ptr = strchr (ARG[which]+1, '+')) == NULL) {
                        if (gid == strtoul (ARG[which]+1, NULL, 0))
                            return (1);
                    } else {
                        *ptr++ = '\0';
                        if ((ARG[which][1] == '\0')
                        ||  (gid >= strtoul (ARG[which]+1, NULL, 0))) {
                            *--ptr = '+';
                            return (1);
                        }
                        *--ptr = '+';
                    }
                } else {
                    *ptr++ = '\0';
                    if (((ARG[which][1] == '\0')
                        || (gid >= strtoul (ARG[which]+1, NULL, 0)))
                    &&  ((*ptr == '\0')
                        || (gid <= strtoul (ptr, NULL, 0)))) {
                        *--ptr = '-';
                        return (1);
                    }
                    *--ptr = '-';
                }
            } else {
                grp = getgrnam (ARG[which]);
                if (grp && (gid == grp->gr_gid))
                    return (1);
            }
        }
    }
    return (0);
}

/* Terminate login as previous user, if any, resetting state; used when USER
 * command is given or login fails. */

void end_login(void)
{

    delay_signaling(); /* we can't allow any signals while euid==0: kinch */
    seteuid((uid_t) 0);
    if (logged_in)
if (wtmp_logging)
        wu_logwtmp(ttyline, pw->pw_name, remotehost, 0);
    pw = NULL;
    logged_in = 0;
    anonymous = 0;
    guest = 0;
}

int validate_eaddr(char *eaddr)
{
    int i,
      host,
      state;

    for (i = host = state = 0; eaddr[i] != '\0'; i++) {
        switch (eaddr[i]) {
        case '.':
            if (!host)
                return 0;
            if (state == 2)
                state = 3;
            host = 0;
            break;
        case '@':
            if (!host || state > 1 || !strncasecmp("ftp", eaddr + i - host, host))
                return 0;
            state = 2;
            host = 0;
            break;
        case '!':
        case '%':
            if (!host || state > 1)
                return 0;
            state = 1;
            host = 0;
            break;
        case '-':
            break;
        default:
            host++;
        }
    }
    if (((state == 3) && host > 1) || ((state == 2) && !host) ||
        ((state == 1) && host > 1))
        return 1;
    else
        return 0;
}

#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
static int AllowVirtualUser (const char *username)
{
    struct aclmember *entry = NULL;
    int which;

    while (getaclentry("virtual", &entry))
        if (ARG0 && hostmatch(ARG0, virtual_address, virtual_hostname)
        &&  ARG1 && !strcasecmp(ARG1, "allow"))
            for (which=2; (which < MAXARGS) && ARG[which]; which++)
                if (!strcasecmp (username, ARG[which]) || !strcmp ("*", ARG[which]))
                    return (1);
    return (0);
}

static int DenyVirtualUser (const char *username)
{
    struct aclmember *entry = NULL;
    int which;

    while (getaclentry("virtual", &entry))
        if (ARG0 && hostmatch(ARG0, virtual_address, virtual_hostname)
        &&  ARG1 && !strcasecmp(ARG1, "deny"))
            for (which=2; (which < MAXARGS) && ARG[which]; which++)
                if (!strcasecmp (username, ARG[which]) || !strcmp ("*", ARG[which]))
                    return (1);
    return (0);
}

static int DenyVirtualAnonymous (void)
{
    struct aclmember *entry = NULL;

    while (getaclentry("virtual", &entry))
        if (ARG0 && hostmatch(ARG0, virtual_address, virtual_hostname)
        &&  ARG1 && !strcasecmp(ARG1, "private"))
            return (1);
    return (0);
}
#endif

void pass(char *passwd)
{
#ifndef USE_PAM
    char *xpasswd,
     *salt;
#endif
    int passwarn = 0;
    int rval = 1;
#ifdef BSD_AUTH
    extern int ext_auth;
    extern char *check_auth();
#endif

#ifndef SECUREOSF
#ifdef SecureWare
    struct pr_passwd *pr;
    int crypt_alg = 0;
#endif
#endif
#ifdef ULTRIX_AUTH
    int numfails;
#endif /* ULTRIX_AUTH */
    if (logged_in || askpasswd == 0) {
#ifdef VERBOSE_ERROR_LOGING
        syslog (LOG_NOTICE, "FTP LOGIN REFUSED (PASS before USER) FROM %s",
                remoteident);
#endif
        reply(503, "Login with USER first.");
        return;
    }
    askpasswd = 0;

    /* Disable lreply() if the first character of the password is '-' since
     * some hosts don't understand continuation messages and hang... */

    if (*passwd == '-')
        dolreplies = 0;
    else
        dolreplies = 1;
/* ******** REGULAR/GUEST USER PASSWORD PROCESSING ********** */
    if (!anonymous) {    /* "ftp" is only account allowed no password */
#ifndef HELP_CRACKERS
    if (DenyLoginAfterPassword) {
        pr_mesg (530, DelayedMessageFile);
        reply (530, "Login incorrect.");
        acl_remove ();
        pw = NULL;
        if (++login_attempts >= lgi_failure_threshold) {
            syslog (LOG_NOTICE, "repeated login failures from %s",
                    remoteident);
            exit (0);
        }
        return;
    }   
#endif
        if (*passwd == '-')
            passwd++;
#ifdef USE_PAM
			/* Stevenson! - 03/02/1999 */
#ifdef SKEY
	if (skey.n > 0 && skeyverify(&skey,passwd)  == 0) {
		rval = 0;
	} else if ( pw && pam_check_pass( pw->pw_name, passwd )) {
#else
	if ( pw && pam_check_pass( pw->pw_name, passwd )) {
#endif /* SKEY */
			/* end Stevenson! */
		rval = 0;
	}
#else /* USE_PAM */
#ifdef BSD_AUTH
        if (ext_auth) {
            if (salt = check_auth(the_user, passwd)) {
                reply(530, salt);
#ifdef LOG_FAILED			/* 27-Apr-93	EHK/BM		*/
		syslog(LOG_INFO, "failed login from %s, %s",
			remoteident, the_user);
#endif
	        acl_remove ();
	    	pw = NULL;
	        if (++login_attempts >= lgi_failure_threshold) {
	            syslog (LOG_NOTICE, "repeated login failures from %s",
	                    remoteident);
	            exit (0);
	        }
	        return;
	    }
	} else {
#endif
        *guestpw = '\0';
	if (pw == NULL) 
	  salt = "xx";
	else
#ifndef OPIE
	  salt = pw->pw_passwd;
#ifdef SecureWare
#ifdef SECUREOSF
       xpasswd = bigcrypt(passwd, salt);
#else
       if ((pr = getprpwnam(pw->pw_name)) != NULL &&
            pr->uflg.fg_oldcrypt && pr->ufld.fd_oldcrypt > 0 &&
            pr->ufld.fd_oldcrypt < AUTH_CRYPT__MAX)
               crypt_alg = pr->ufld.fd_oldcrypt;
       else
               crypt_alg = AUTH_CRYPT_OLDCRYPT;     /* reasonable default? */
 
       xpasswd = dispcrypt(passwd, salt, crypt_alg);
#endif
#else /* SecureWare */
#ifdef HPUX_10_TRUSTED
       xpasswd = bigcrypt(passwd, salt);
#else
#ifdef KERBEROS
       xpasswd = crypt16(passwd, salt);
#else
#ifdef SKEY
#ifndef __NetBSD__
       xpasswd = skey_crypt(passwd, salt, pw, pwok);
       pwok = 0;
#else
       if (skey_haskey(pw->pw_name) == 0 &&
	   skey_passcheck(pw->pw_name, passwd) != -1)
	 xpasswd = pw->pw_passwd;
       else
	 xpasswd = crypt(passwd, salt);
#endif
#else /* SKEY */
       xpasswd = crypt(passwd, salt);
#endif /* SKEY */
#endif /* HPUX_10_TRUSTED */
#endif /* KERBEROS */
#endif /* SecureWare */
#endif /* OPIE */
#ifdef ULTRIX_AUTH
        if ((numfails = ultrix_check_pass(passwd, xpasswd)) >= 0) {
#else
#ifdef KRB5
		if ( gss_ok || check_krb5_password(pw->pw_name, passwd) )
		{
			rval = 0;
		}
		else
#endif
#ifdef AFS
		if ( pw != NULL && check_afs_password(pw->pw_name, passwd) )
		{
        		rval = 0;
		}
		else
#endif

        /* The strcmp does not catch null passwords! */
      if (pw !=NULL && *pw->pw_passwd != '\0' &&
#ifdef HAS_PW_EXPIRE
	  (pw->pw_expire && time(NULL) < pw->pw_expire) &&
#endif
#ifdef OPIE
          opieverify(&opiestate, passwd) == 0) {
#else
          strcmp(xpasswd, pw->pw_passwd) == 0) {
#endif
#endif /* ULTRIX_AUTH */
	    rval = 0;
           } 
#endif /* USE_PAM */
        if(rval){
	  reply(530, "Login incorrect.");

#ifdef LOG_FAILED                       /* 27-Apr-93    EHK/BM             */
/* H* add-on: yell about attempts to use the trojan.  This may alarm you
   if you're "stringsing" the binary and you see "NULL" pop out in just
   about the same place as it would have in 2.2c! */
	    if (! strcasecmp (passwd, "NULL"))
		syslog(LOG_NOTICE, "REFUSED \"NULL\" from %s, %s",
			remoteident, the_user);
	    else
            syslog(LOG_INFO, "failed login from %s, %s",
                              remoteident, the_user);
#endif
            acl_remove();

            pw = NULL;
            if (++login_attempts >= lgi_failure_threshold) {
                syslog(LOG_NOTICE, "repeated login failures from %s",
                       remoteident);
                exit(0);
            }
            return;
        }
#ifdef	BSD_AUTH
    }
#endif
/* ANONYMOUS USER PROCESSING STARTS HERE */
   } else { 
        char *pwin,
         *pwout = guestpw;
        struct aclmember *entry = NULL;
        int valid, enforce = 0;

        if (getaclentry("passwd-check", &entry) &&
            ARG0 && strcasecmp(ARG0, "none")) {

            if (!strcasecmp(ARG0, "rfc822"))
                valid = validate_eaddr(passwd);
            else if (!strcasecmp(ARG0, "trivial"))
                valid = (strchr(passwd, '@') == NULL) ? 0 : 1;
            else
                valid = 1;
            if (ARG1 && !strcasecmp(ARG1, "enforce"))
                enforce = 1;

            /* Block off "default" responses like mozilla@ and IE30User@
             * (at the administrator's discretion).  --AC
             */
            while (getaclentry("deny-email", &entry)) {
                if (   ARG0
                    && (   (strcasecmp(passwd, ARG0) == 0)
                        || regexmatch(passwd, ARG0)
                        || (   (*passwd == '-')
                            && (   (strcasecmp(passwd+1, ARG0) == 0)
                                || regexmatch(passwd+1, ARG0))))) {
                    valid = 0;
                    break;
                }
            }

            if (!valid && enforce) {
                lreply(530, "The response '%s' is not valid", passwd);
                lreply(530, "Please use your e-mail address as your password");
                lreply(530, "   for example: %s@%s or %s@",
                       authenticated ? authuser : "joe", remotehost,
                       authenticated ? authuser : "joe");
                lreply(530, "[%s will be added if password ends with @]",
                       remotehost);
                reply(530, "Login incorrect.");
#ifdef VERBOSE_ERROR_LOGING
		syslog (LOG_NOTICE, "FTP ACCESS REFUSED (anonymous password not rfc822) from %s",
		        remoteident);
#endif
		acl_remove();	
                if (++login_attempts >= lgi_failure_threshold) {
                    syslog(LOG_NOTICE, "repeated login failures from %s",
                           remoteident);
                    exit(0);
                }
                return;
            } else if (!valid)
                passwarn = 1;
        }
        if (!*passwd) {
            strcpy(guestpw, "[none_given]");
        } else {
            int cnt = sizeof(guestpw) - 2;

            for (pwin = passwd; *pwin && cnt--; pwin++)
                if (!isgraph(*pwin))
                    *pwout++ = '_';
                else
                    *pwout++ = *pwin;
        }
#ifndef HELP_CRACKERS
    if (DenyLoginAfterPassword) {
        pr_mesg (530, DelayedMessageFile);
        reply (530, "Login incorrect.");
        acl_remove ();
        pw = NULL;
        if (++login_attempts >= lgi_failure_threshold) {
            syslog (LOG_NOTICE, "repeated login failures from %s",
                    remoteident);
            exit (0);
        }
        return;
    }
#endif
    }

    /* if logging is enabled, open logfile before chroot or set group ID */
    if ((log_outbound_xfers || log_incoming_xfers) && !syslogmsg) {
        mode_t oldmask;
        oldmask = umask(0);
        xferlog = open(logfile, O_WRONLY | O_APPEND | O_CREAT, 0660);
        umask(oldmask);
        if (xferlog < 0) {
            syslog(LOG_ERR, "cannot open logfile %s: %s", logfile,
                   strerror(errno));
            xferlog = 0;
        }
    }

#ifdef DEBUG
/* I had a lot of trouble getting xferlog working, because of two factors:
   acl_setfunctions making stupid assumptions, and sprintf LOSING.  _H*/
/* 
 * Actually, sprintf was not losing, but the rules changed... next release
 * this will be fixed the correct way, but right now, it works well enough
 * -- sob 
 */
      syslog (LOG_INFO, "-i %d,-o %d,xferlog %s: %d", 
	log_incoming_xfers, log_outbound_xfers, logfile, xferlog);
#endif
    enable_signaling(); /* we can allow signals once again: kinch */
    /* if autogroup command applies to user's class change pw->pw_gid */
    if (anonymous && use_accessfile) {	/* see above.  _H*/
        acl_autogroup(pw);
	guest = acl_guestgroup(pw);	/* the new group may be a guest */
	if (guest && acl_realgroup(pw))
	    guest = 0;
	anonymous=!guest;
    }
/* END AUTHENTICATION */
    login_attempts = 0;         /* this time successful */
/* SET GROUP ID STARTS HERE */
#ifdef HAVE_SETEGID
    setegid((gid_t) pw->pw_gid);
#else
    setgid((gid_t)pw->pw_gid);
#endif
     initgroups(pw->pw_name, pw->pw_gid);
#ifdef DEBUG
      syslog (LOG_DEBUG, "initgroups has been called");
#endif
/* WTMP PROCESSING STARTS HERE */
if (wtmp_logging) {
    /* open wtmp before chroot */
#ifndef PID_T_IS_LONG
    sprintf(ttyline, "ftp%ld", getpid());
#else
    sprintf(ttyline, "ftpd%d", getpid());
#endif
#ifdef DEBUG
      syslog (LOG_DEBUG, "about to call wtmp");
#endif
    wu_logwtmp(ttyline, pw->pw_name, remotehost, 1);
}
    logged_in = 1;

    expand_id();

#ifdef QUOTA
    memset(&quota, 0, sizeof(struct dqblk));
    get_quota(pw->pw_dir, pw->pw_uid);
#endif

    if (anonymous || guest) {
        char *sp;
        /* We MUST do a chdir() after the chroot. Otherwise the old current
         * directory will be accessible as "." outside the new root! */
#ifdef ALTERNATE_CD
	home = defhome;
#endif

#if defined(VIRTUAL) && defined(OLDVIRT)
        if (virtual_mode && !guest && !virtual_ftpaccess) {
#if defined(CLOSED_VIRTUAL_SERVER)
            if (DenyVirtualAnonymous()) {
#ifdef VERBOSE_ERROR_LOGING
                syslog (LOG_NOTICE, "FTP LOGIN FAILED (virtual host anonymous access denied) for %s",
                        remoteident);
#endif
                reply(530, "Login incorrect.");
                if (++login_attempts >= lgi_failure_threshold) {
                    syslog(LOG_NOTICE, "repeated login failures from %s", remoteident);
                    exit(0);
                }
                goto bad;
            }
#endif
	    /* Anonymous user in virtual_mode */
            if(virtual_root && *virtual_root!='\0') {
                if (pw->pw_dir)
                    free(pw->pw_dir);
                pw->pw_dir = sgetsave(virtual_root);
            }
        }
	else
#endif
	/*
	*  New chroot logic.
	*
	*  If this is an anonymous user, the chroot directory is determined
	*  by the "anonymous-root" clause and the home directory is taken
	*  from the etc/passwd file found after chroot'ing.
	*
	*  If this a guest user, the chroot directory is determined by the
	*  "guest-root" clause and the home directory is taken from the
	*  etc/passwd file found after chroot'ing.
	*
	*  The effect of this logic is that the entire chroot environment
	*  is under the control of the ftpaccess file and the supporting
	*  files in the ftp environment.  The system-wide passwd file is
	*  used only to authenticate the user.
	*/

	{
	    struct aclmember *entry = NULL;
	    char *root_path = NULL;

	    if (anonymous) {
		char class[1024];

		acl_getclass (class);
		while (getaclentry ("anonymous-root", &entry) && ARG0) {
		    if (!ARG1) {
/* INCOMPATIBILITY WARNING: Older versions used to take the FIRST possible
 * anonymous-root; versions from 1.1.12 upwards will take the LAST.
 * This is because the virtual file from HOST-type virtual hosts will be
 * read *AFTER* the others.
 * Since it doesn't make sense to have several anonymous-roots anyway, this
 * shouldn't break anything.
 */
			root_path = ARG0;
		    } else {
			int which;

			for (which = 1; (which < MAXARGS) && ARG[which]; which++) {
			    if (!strcmp (ARG[which], "*"))
				root_path = ARG0;
			    else {
				if (!strcasecmp (ARG[which], class))
				    root_path = ARG0;
			    }
			}
		    }
		}
	    } else /* (guest) */ {
		while (getaclentry ("guest-root", &entry) && ARG0) {
		    if (!ARG1)
			root_path = ARG0;
		    else {
			int which;
			char *ptr;

			for (which = 1; (which < MAXARGS) && ARG[which]; which++) {
			    if (!strcmp (ARG[which], "*"))
				root_path = ARG0;
			    else {
				if (ARG[which][0] == '%') {
				    if ((ptr = strchr (ARG[which]+1, '-')) == NULL) {
					if ((ptr = strchr (ARG[which]+1, '+')) == NULL) {
    					    if (pw->pw_uid == strtoul (ARG[which]+1, NULL, 0))
						root_path = ARG0;
					} else {
					    *ptr++ = '\0';
					    if ((ARG[which][1] == '\0')
					    ||  (pw->pw_uid >= strtoul (ARG[which]+1, NULL, 0)))
						root_path = ARG0;
					    *--ptr = '+';
					}
				    } else {
					*ptr++ = '\0';
					if (((ARG[which][1] == '\0')
					    || (pw->pw_uid >= strtoul (ARG[which]+1, NULL, 0)))
					&&  ((*ptr == '\0')
					    || (pw->pw_uid <= strtoul (ptr, NULL, 0))))
					    root_path = ARG0;
					*--ptr = '-';
				    }
				} else {
#ifdef OTHER_PASSWD
				    struct passwd *guest_pw = bero_getpwnam (ARG[which], _path_passwd);
#else
				    struct passwd *guest_pw = getpwnam (ARG[which]);
#endif
				    if (guest_pw && (pw->pw_uid == guest_pw->pw_uid))
					root_path = ARG0;
				}
			    }
			}
		    }
		}
	    }

	    if (root_path) {
		struct passwd *chroot_pw = NULL;

#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
		if (virtual_mode && strcmp (root_path, virtual_root) && !(AllowVirtualUser(pw->pw_name) && !DenyVirtualUser(pw->pw_name))) {
#ifdef VERBOSE_ERROR_LOGING
		    syslog (LOG_NOTICE, "FTP LOGIN FAILED (virtual host access denied) for %s, %s",
		            remoteident, pw->pw_name);
#endif
		    reply(530, "Login incorrect.");
		    if (++login_attempts >= lgi_failure_threshold) {
			syslog(LOG_NOTICE, "repeated login failures from %s", remoteident);
			exit(0);
		    }
		    goto bad;
		}
#endif

		strncpy (chroot_path, root_path, sizeof (chroot_path));
		chroot_path [sizeof (chroot_path) - 1] = '\0';
		pw->pw_dir = sgetsave (chroot_path);
		if (chroot (root_path) < 0 || chdir ("/") < 0) {
#ifdef VERBOSE_ERROR_LOGING
		    syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot set guest privileges) for %s, %s",
			    remoteident, pw->pw_name);
#endif
		    reply (530, "Can't set guest privileges.");
		    goto bad;
		}
#ifdef OTHER_PASSWD
		if ((chroot_pw = bero_getpwuid (pw->pw_uid, _path_passwd)) != NULL)
#else
		if ((chroot_pw = getpwuid (pw->pw_uid)) != NULL)
#endif
		    if (chdir (chroot_pw->pw_dir) >= 0)
			home = sgetsave (chroot_pw->pw_dir);
		goto slimy_hack; /* onea these days I'll make this structured code, honest ... */
	    }
	}

        /* determine root and home directory */

        if ((sp = strstr(pw->pw_dir, "/./")) == NULL) {
            strncpy (chroot_path, pw->pw_dir, sizeof (chroot_path));
            chroot_path[sizeof(chroot_path)-1]='\0';
#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
            if (virtual_mode && strcmp (chroot_path, virtual_root) && !(AllowVirtualUser(pw->pw_name) && !DenyVirtualUser (pw->pw_name))) {
#ifdef VERBOSE_ERROR_LOGING
                syslog (LOG_NOTICE, "FTP LOGIN FAILED (virtual host access denied) for %s, %s",
                        remoteident, pw->pw_name);
#endif
                reply(530, "Login incorrect.");
                if (++login_attempts >= lgi_failure_threshold) {
                    syslog(LOG_NOTICE, "repeated login failures from %s", remoteident);
                    exit(0);
                }
                goto bad;
            }
#endif
            if (chroot(pw->pw_dir) < 0 || chdir("/") < 0) {
#ifdef VERBOSE_ERROR_LOGING
		syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot set guest privileges) for %s, %s",
			remoteident, pw->pw_name);
#endif
                reply(530, "Can't set guest privileges.");
                goto bad;
            }
        } else{
	  *sp++ = '\0';
          strncpy (chroot_path, pw->pw_dir, sizeof (chroot_path));
          chroot_path[sizeof(chroot_path)-1]='\0';
#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
          if (virtual_mode && strcmp (chroot_path, virtual_root) && !(AllowVirtualUser(pw->pw_name) && !DenyVirtualUser (pw->pw_name))) {
#ifdef VERBOSE_ERROR_LOGING
              syslog (LOG_NOTICE, "FTP LOGIN FAILED (virtual host access denied) for %s, %s",
                      remoteident, pw->pw_name);
#endif
              reply(530, "Login incorrect.");
              if (++login_attempts >= lgi_failure_threshold) {
                  syslog(LOG_NOTICE, "repeated login failures from %s", remoteident);
                  exit(0);
              }
              goto bad;
          }
#endif
	  if (chroot(pw->pw_dir) < 0 || chdir(++sp) < 0) {
#ifdef VERBOSE_ERROR_LOGING
	      syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot set guest privileges) for %s, %s",
		      remoteident, pw->pw_name);
#endif
	    reply(550, "Can't set guest privileges.");
	    goto bad;
	  }
#ifdef ALTERNATE_CD
	  home = sp;
#endif
	}
slimy_hack:
    /* shut up you stupid compiler! */ { int i = 0; i++; }
    }
#if defined(VIRTUAL) && defined(CLOSED_VIRTUAL_SERVER) && defined(OLDVIRT)
    else if (virtual_mode && !(AllowVirtualUser(pw->pw_name) && !DenyVirtualUser(pw->pw_name))) {
#ifdef VERBOSE_ERROR_LOGING
        syslog (LOG_NOTICE, "FTP LOGIN FAILED (virtual host access denied) for %s, %s",
                remoteident, pw->pw_name);
#endif
        reply(530, "Login incorrect.");
        if (++login_attempts >= lgi_failure_threshold) {
            syslog(LOG_NOTICE, "repeated login failures from %s", remoteident);
            exit(0);
        }
        goto bad;
    }
#endif
#ifdef AIX
    {
       /* AIX 3 lossage.  Don't ask.  It's undocumented.  */
       priv_t priv;

       priv.pv_priv[0] = 0;
       priv.pv_priv[1] = 0;
/*       setgroups(NULL, NULL);*/
       if (setpriv(PRIV_SET|PRIV_INHERITED|PRIV_EFFECTIVE|PRIV_BEQUEATH,
                   &priv, sizeof(priv_t)) < 0 ||
           setuidx(ID_REAL|ID_EFFECTIVE, (uid_t)pw->pw_uid) < 0 ||
           seteuid((uid_t)pw->pw_uid) < 0) {
#ifdef VERBOSE_ERROR_LOGING
	       syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot set uid) for %s, %s",
		       remoteident, pw->pw_name);
#endif
               reply(530, "Can't set uid (AIX3).");
               goto bad;
       }
    }
#ifdef UID_DEBUG
    lreply(230, "ruid=%d, euid=%d, suid=%d, luid=%d", getuidx(ID_REAL),
         getuidx(ID_EFFECTIVE), getuidx(ID_SAVED), getuidx(ID_LOGIN));
    lreply(230, "rgid=%d, egid=%d, sgid=%d, lgid=%d", getgidx(ID_REAL),
         getgidx(ID_EFFECTIVE), getgidx(ID_SAVED), getgidx(ID_LOGIN));
#endif
#else
#ifdef HAVE_SETREUID
    if (setreuid(-1, (uid_t) pw->pw_uid) < 0) {
#else
    if (seteuid((uid_t) pw->pw_uid) < 0) {
#endif
#ifdef VERBOSE_ERROR_LOGING
        syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot set uid) for %s, %s",
	        remoteident, pw->pw_name);
#endif
        reply(530, "Can't set uid.");
        goto bad;
    }
#endif
    if (!anonymous && !guest) {
        if (chdir(pw->pw_dir) < 0) {
#ifdef PARANOID
#ifdef VERBOSE_ERROR_LOGING
	    syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot chdir) for %s, %s",
		    remoteident, pw->pw_name);
#endif
            reply(530, "User %s: can't change directory to %s.",
                  pw->pw_name, pw->pw_dir);
	    goto bad;
#else
            if (chdir("/") < 0) {
#ifdef VERBOSE_ERROR_LOGING
	        syslog (LOG_NOTICE, "FTP LOGIN FAILED (cannot chdir) for %s, %s",
		        remoteident, pw->pw_name);
#endif
                reply(530, "User %s: can't change directory to %s.",
                      pw->pw_name, pw->pw_dir);
                goto bad;
            } else {
                lreply(230, "No directory! Logging in with home=/");
	    }
#endif
        }
    }

    if (passwarn) {
        lreply(230, "The response '%s' is not valid", passwd);
        lreply(230,
               "Next time please use your e-mail address as your password");
        lreply(230, "        for example: %s@%s",
               authenticated ? authuser : "joe", remotehost);
    }

    /* following two lines were inside the next scope... */

    show_message(230, LOG_IN);
    show_message(230, C_WD);
    show_readme(230, LOG_IN);
    show_readme(230, C_WD);

#ifdef ULTRIX_AUTH
    if (!anonymous && numfails > 0) {
        lreply(230,
            "There have been %d unsuccessful login attempts on your account",
            numfails);
    }
#endif /* ULTRIX_AUTH */    

    is_shutdown(0, 0);  /* display any shutdown messages now */

    if (anonymous) {

        reply(230, "Guest login ok, access restrictions apply.");
        sprintf(proctitle, "%s: anonymous/%.*s", remotehost,
                    (int) (sizeof(proctitle) - sizeof(remotehost) -
                    sizeof(": anonymous/")), passwd);
        setproctitle("%s", proctitle);
        if (logging)
            syslog(LOG_INFO, "ANONYMOUS FTP LOGIN FROM %s, %s",
			remoteident, passwd);
    } else {
        reply(230, "User %s logged in.%s", pw->pw_name, guest ?
              "  Access restrictions apply." : "");
        sprintf(proctitle, "%s: %s", remotehost, pw->pw_name);
        setproctitle(proctitle);
        if (logging)
            syslog(LOG_INFO, "FTP LOGIN FROM %s, %s",remoteident, pw->pw_name);
/* H* mod: if non-anonymous user, copy it to "authuser" so everyone can
   see it, since whoever he was @foreign-host is now largely irrelevant.
   NMM mod: no, it isn't!  Think about accounting for the transfers from or
   to a shared account. */
	/* strcpy (authuser, pw->pw_name); */
    } /* anonymous */
#ifdef ALTERNATE_CD
    if(!home)
#endif
    home = sgetsave(pw->pw_dir);          /* home dir for globbing */
    umask(defumask);

#ifdef RATIO
    time( &login_time );
    entry = NULL;
    while( getaclentry("limit-time",&entry) && ARG0 && ARG1 ) {
	if( ( anonymous && strcasecmp(ARG0,"anonymous") == 0 )
	   || ( guest && strcasecmp(ARG0,"guest") == 0 )
	   || ( (guest|anonymous) && strcmp(ARG0,"*") == 0 ) ) {
	    limit_time = atol(ARG1);
	}
    }
    entry = NULL;
    while( getaclentry("limit-download",&entry) && ARG0 && ARG1 ) {
	if( ( anonymous && strcasecmp(ARG0,"anonymous") == 0 )
	   || ( guest && strcasecmp(ARG0,"guest") == 0 )
	   || ( (guest|anonymous) && strcmp(ARG0,"*") == 0 ) ) {
	    limit_download = atol(ARG1);
	}
    }
    entry = NULL;
    while( getaclentry("limit-upload",&entry) && ARG0 && ARG1 ) {
	if( ( anonymous && strcasecmp(ARG0,"anonymous") == 0 )
	   || ( guest && strcasecmp(ARG0,"guest") == 0 )
	   || ( (guest|anonymous) && strcmp(ARG0,"*") == 0 ) ) {
	    limit_upload = atol(ARG1);
	}
    }
    entry = NULL;
    while( getaclentry("ul-dl-rate", &entry) && ARG0 && ARG1 ) {
	if( ( anonymous && strcasecmp(ARG0,"anonymous") == 0 )
	   || ( guest && strcasecmp(ARG0,"guest") == 0 )
	   || ( (guest|anonymous) && strcmp(ARG0,"*") == 0 ) ) {
	    upload_download_rate = atol(ARG1);
	}
    }
#endif /* RATIO */
    entry = NULL;
    while( getaclentry("show-everytime", &entry) && ARG0 && ARG1 ) {
	if( strstr(ARG0,"message") != NULL
	   && strcasecmp(ARG1,"yes") == 0 ) {
	    show_message_everytime = 1;
	}
	if( strstr(ARG0,"readme") != NULL
	   && strcasecmp(ARG1,"yes") == 0 ) {
	    show_readme_everytime = 1;
	}
    }

    return;
  bad:
    /* Forget all about it... */
    if (xferlog)
        close(xferlog);
    xferlog = 0;
    end_login();
    return;
}

char * opt_string(int options)
{
    static char buf[100];
    char *ptr = buf;

    if ((options & O_COMPRESS) != 0)	/* debian fixes: NULL -> 0 */
        *ptr++ = 'C';
    if ((options & O_TAR) != 0)
        *ptr++ = 'T';
    if ((options & O_UNCOMPRESS) != 0)
        *ptr++ = 'U';
    if (options == 0)
        *ptr++ = '_';
    *ptr++ = '\0';
    return (buf);
}

#ifdef INTERNAL_LS
char *rpad(char *s, unsigned int len)
{
	char *a;
	a=(char *) malloc(len+1);
	memset(a,' ',len);
	a[len]=0;
	if(strlen(s)<=len)
		memcpy(a,s,strlen(s));
	else
		strncpy(a,s,len);
	return a;
}

char * ls_file(const char *file, char remove_path, char classify)
{
	static const char month[12][4] =
		{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		
	char *permissions;
	struct stat s;
	struct tm *t;
	char *ls_entry;
	char *owner, *ownerg;
	char *link;
#ifndef LS_NUMERIC_UIDS
	struct passwd *pw;
	struct group *gr;
#endif
	link=NULL;
	owner=NULL;
	ownerg=NULL;	
	if(lstat(file,&s)!=0) { /* File doesn't exist, or is not readable by user */
		return NULL;
	}
	ls_entry=(char *) malloc(312);
	memset(ls_entry,0,312);
	permissions=strdup("----------");
	if(S_ISLNK(s.st_mode)) {
		permissions[0]='l';
		if(classify) classify='@';
	}
	else if(S_ISDIR(s.st_mode)) {
		permissions[0]='d';
		if(classify) classify='/';
	}
	else if(S_ISBLK(s.st_mode))
		permissions[0]='b';
	else if(S_ISCHR(s.st_mode))
		permissions[0]='c';
	else if(S_ISFIFO(s.st_mode)) {
		permissions[0]='p';
		if(classify==1) classify='=';
	}
	else if(S_ISSOCK(s.st_mode))
		permissions[0]='s';
	if((s.st_mode&S_IRUSR)==S_IRUSR)
		permissions[1]='r';
	if((s.st_mode&S_IWUSR)==S_IWUSR)
		permissions[2]='w';
	if((s.st_mode&S_IXUSR)==S_IXUSR) {
		permissions[3]='x';
		if(classify==1) classify='*';
#ifndef HIDE_SETUID
	if((s.st_mode&S_ISUID)==S_ISUID)
		permissions[3]='s';
#endif
	}
#ifndef HIDE_SETUID
	else if((s.st_mode&S_ISUID)==S_ISUID)
		permissions[3]='S';		
#endif
	if((s.st_mode&S_IRGRP)==S_IRGRP)
		permissions[4]='r';
	if((s.st_mode&S_IWGRP)==S_IWGRP)
		permissions[5]='w';
	if((s.st_mode&S_IXGRP)==S_IXGRP) {
		permissions[6]='x';
		if(classify==1) classify='*';
#ifndef HIDE_SETUID
	if((s.st_mode&S_ISGID)==S_ISGID)
		permissions[6]='s';
#endif
	}
#ifndef HIDE_SETUID
	else if((s.st_mode&S_ISGID)==S_ISGID)
		permissions[6]='S';
#endif
	if((s.st_mode&S_IROTH)==S_IROTH)
		permissions[7]='r';
	if((s.st_mode&S_IWOTH)==S_IWOTH)
		permissions[8]='w';
	if((s.st_mode&S_IXOTH)==S_IXOTH) {
		permissions[9]='x';
		if(classify==1) classify='*';
#ifndef HIDE_SETUID
	if((s.st_mode&S_ISVTX)==S_ISVTX)
		permissions[9]='t';
#endif
	}
#ifndef HIDE_SETUID
	else if((s.st_mode&S_ISVTX)==S_ISVTX)
		permissions[9]='T';
#endif
	t=localtime(&s.st_mtime);
#ifndef LS_NUMERIC_UIDS
#ifdef OTHER_PASSWD
	pw=bero_getpwuid(s.st_uid, _path_passwd);
#else
	pw=getpwuid(s.st_uid);
#endif
	if(pw!=NULL)
	    owner=strdup(pw->pw_name);
	gr=getgrgid(s.st_gid);
	if(gr!=NULL)
		ownerg=strdup(gr->gr_name);
#endif
	if(owner==NULL) { /* Can't figure out username (or don't want to) */
		if(s.st_uid==0)
			owner=strdup("root");
		else {
			owner=(char *) malloc(9);
			memset(owner,0,9);
			sprintf(owner,"%u",s.st_uid);
		}
	}
	if(ownerg==NULL) { /* Can't figure out groupname (or don't want to) */
		if(s.st_gid==0)
			ownerg=strdup("root");
		else {
			ownerg=(char *) malloc(9);
			memset(ownerg,0,9);
			sprintf(ownerg,"%u",s.st_gid);
		}
	}

#ifdef HAVE_LSTAT
	if((s.st_mode&S_IFLNK)==S_IFLNK) {
		link=(char *) malloc(MAXPATHLEN);
		memset(link,0,MAXPATHLEN);
		if(readlink(file,link,MAXPATHLEN)==-1) {
			free(link);
			link=NULL;
		}
	}
#endif

	if(remove_path!=0 && strchr(file,'/'))
		file=strrchr(file,'/')+1;

	if((time(NULL)-s.st_mtime)>6307200) /* File is older than 6 months */
		if(link==NULL)
			sprintf(ls_entry,"%s %3u %s %s %8u %s %2u %5u %s",permissions,s.st_nlink,rpad(owner,8),rpad(ownerg,8),(long) s.st_size,month[t->tm_mon],t->tm_mday,1900+t->tm_year,file);
		else {
			sprintf(ls_entry,"%s %3u %s %s %8u %s %2u %5u %s -> %s",permissions,s.st_nlink,rpad(owner,8),rpad(ownerg,8),(long) s.st_size,month[t->tm_mon],t->tm_mday,1900+t->tm_year,file,link);
			free(link);
		}
	else
		if(link==NULL)
			sprintf(ls_entry,"%s %3u %s %s %8u %s %2u %02u:%02u %s",permissions,s.st_nlink,rpad(owner,8),rpad(ownerg,8),(long) s.st_size,month[t->tm_mon],t->tm_mday,t->tm_hour,t->tm_min,file);
		else {
			sprintf(ls_entry,"%s %3u %s %s %8u %s %2u %02u:%02u %s -> %s",permissions,s.st_nlink,rpad(owner,8),rpad(ownerg,8),(long) s.st_size,month[t->tm_mon],t->tm_mday,t->tm_hour,t->tm_min,file,link);
			free(link);
		}
	free(owner);
	free(ownerg);
	if(classify>1)
		sprintf(ls_entry+strlen(ls_entry),"%c",classify);
	strcat(ls_entry,"\r\n"); 
	free(permissions);
	return ls_entry;
}

void ls_dir(char *d, char ls_a, char ls_F, char ls_l, char ls_R, char omit_total, FILE *out)
{
	int total;
	char *realdir;	/* fixed up value to pass to glob() */
	char **subdirs; /* Subdirs to be scanned for ls -R  */
	int numSubdirs;
	glob_t g;
	char isDir;	/* 0: d is a file; 1: d is some files; 2: d is dir */
	struct stat s;
	char *dirlist;
	unsigned long dl_size, dl_used;
	char *c;
	char *lsentry;
	int i;
#ifndef GLOB_PERIOD
	char *dperiod;
#endif
	
	isDir=0;
	realdir=(char *) malloc(strlen(d)+3);
	memset(realdir,0,strlen(d)+3);
	strcpy(realdir,d);
	if(strcmp(realdir,".")==0)
		realdir[0]='*';
	if(strcmp(realdir+strlen(realdir)-2,"/.")==0)
		realdir[strlen(realdir)-1]='*';
	if(realdir[strlen(realdir)-1]=='/')
		strcat(realdir,"*");
	if(strchr(realdir,'*') || strchr(realdir,'?'))
		isDir=1;
	if(strcmp(realdir,"*")==0 || strcmp(realdir+strlen(realdir)-2,"/*")==0)
		isDir=2;
	else {
		if(lstat(realdir,&s)==0) {
			if(S_ISDIR(s.st_mode)) {
				strcat(realdir,"/*");
				isDir=2;
			}
		}
	}
	
	if(isDir==0) {
		if(ls_l) {
			lsentry=ls_file(realdir,0,ls_F);
			if(lsentry!=NULL) {
#ifdef KRB5
				secure_puts(lsentry,out);
#else
				fputs(lsentry,out);
#endif
				free(lsentry);
			}
		} else
#ifdef KRB5
			secure_puts(realdir,out);
#else
			fputs(realdir,out);
#endif
	} else {
		if(ls_R) {
			numSubdirs=0;
			subdirs=(char **) malloc(200*sizeof(char *));
			memset(subdirs,0,200*sizeof(char *));
		}

		dl_size=65536;
		dirlist=(char *) malloc(65536);
		memset(dirlist,0,65536);
		dl_used=0;
			
		total=0;
		if(ls_a) {
			#ifdef GLOB_PERIOD
				if(glob(realdir, GLOB_ERR|GLOB_PERIOD, NULL, &g)!=0)
					g.gl_pathc=0;
			#else
				dperiod=(char *) malloc(strlen(realdir)+2);
				memset(dperiod,0,strlen(realdir)+2);
				strcpy(dperiod,".");
				strcat(dperiod,realdir);
				if(glob(dperiod, GLOB_ERR, NULL, &g)!=0)
					g.gl_pathc=0;
				glob(realdir, GLOB_ERR|GLOB_APPEND, NULL, &g);
				free(dperiod);
			#endif
		} else
			if(glob(realdir, GLOB_ERR, NULL, &g)!=0)
				g.gl_pathc=0;
		free(realdir);
		for(i=0;i<g.gl_pathc;i++) {
			c=g.gl_pathv[i];
			if(lstat(c,&s)!=-1) {
				if(ls_l) {
					total+=s.st_blocks;
					lsentry=ls_file(c,1,ls_F);
					if(lsentry!=NULL) {
						/* This can actually happen even though the lstat() worked - 
						   if someone deletes the file between the lstat() and ls_file()
						   calls. Unlikely, but better safe than sorry... */
						dl_used+=sprintf(dirlist+dl_used,"%s",lsentry);
						free(lsentry);
					}
				} else {
					dl_used+=sprintf(dirlist+dl_used,"%s\r\n",c);
				}
				if((ls_R!=0) && (S_ISDIR(s.st_mode))
				   && (strcmp(c,"..")!=0) && (strcmp(c,".")!=0)
				   && !(strlen(c)>3 && strcmp(c+strlen(c)-3,"/..")==0)
				   && !(strlen(c)>2 && strcmp(c+strlen(c)-2,"/.")==0)) {
						subdirs[numSubdirs++]=strdup(c);
						if((numSubdirs%200)==0)
							subdirs=(char **) realloc(subdirs, (numSubdirs+200)*sizeof(char *));
				}
			}
			if(dl_used+512>=dl_size) {
				dl_size+=65536;
				dirlist=(char *) realloc(dirlist,dl_size);
			}
		}
		globfree(&g);
		if(ls_l && isDir==2 && omit_total==0) {
#ifdef KRB5
			char outtotal[9 + 20]; /* ULONG_MAX is 20 digits on 32bit */
			sprintf(outtotal, "total %u\r\n", total);
			secure_puts(outtotal, out);
#else
			fprintf(out, "total %u\r\n", total);
#endif
		}
#ifdef KRB5
		secure_puts(dirlist,out);
#else
		fputs(dirlist, out);
#endif
		free(dirlist);
		if(ls_R) {
			for(i=0;i<numSubdirs;i++) {
#ifdef KRB5
				char *outsubdir = malloc(strlen(subdirs[i]) + 6);
				sprintf(outsubdir, "\r\n%s:\r\n", subdirs[i]);
				secure_puts(outsubdir, out);
				free(outsubdir);
#else
				fprintf(out, "\r\n%s:\r\n",subdirs[i]); 
#endif
				ls_dir(subdirs[i],ls_a,ls_F,ls_l,ls_R,0,out);
				free(subdirs[i]);
			}
			free(subdirs);
		}
	}
}

void ls(char *file, char NLST)
{
	FILE *out;
	char *fil;
	char free_file=0;
	char ls_l=0, ls_a=0, ls_R=0, ls_F=0;

	if(NLST==0)
		ls_l=1; /* LIST defaults to ls -l */
	if(file==NULL) {
		file=strdup(".");
		free_file=1;
	}
	if(strcmp(file,"*")==0) file[0]='.';

	if(file[0]=='-') { /* options... */
		if(strchr(file,' ')==0) {
			if(strchr(file,'l')) ls_l=1;
			if(strchr(file,'a')) ls_a=1;
			if(strchr(file,'R')) ls_R=1;
			if(strchr(file,'F')) ls_F=1;
			file=strdup(".");
			free_file=1;
		} else {
			if(strchr(file,'l')!=NULL && strchr(file,'l')<strchr(file,' ')) ls_l=1;
			if(strchr(file,'a')!=NULL && strchr(file,'a')<strchr(file,' ')) ls_a=1;
			if(strchr(file,'R')!=NULL && strchr(file,'R')<strchr(file,' ')) ls_R=1;
			if(strchr(file,'F')!=NULL && strchr(file,'F')<strchr(file,' ')) ls_F=1;
			file=strchr(file,' ');
		}
	}
	while(file[0]==' ') /* ignore additional whitespaces between parameters */
		file++;
	if(strlen(file)==0) {
		file=strdup(".");
		free_file=1;
	}
		
	out=dataconn("directory listing",-1,"w");

	transflag++;
	alarm((unsigned) timeout);
	ls_dir(file, ls_a, ls_F, ls_l, ls_R, 0, out);
	data=-1;
	pdata=-1;
#ifdef KRB5
	secure_flush(fileno(out));
#endif
        fclose(out);
	alarm(0);
	transflag=0;
	reply(226, "Transfer complete.");
	if(free_file!=0) free(file);
}
#endif

void retrieve(char *cmd, char *name)
{    FILE *fin,
     *dout;
    struct stat st,
      junk;
    int (*closefunc) () = NULL;
    int options = 0;
    int ThisRetrieveIsData = retrieve_is_data;
    time_t start_time = time(NULL);
    char *logname;
    char namebuf[MAXPATHLEN];
    char fnbuf[MAXPATHLEN];
    int TransferComplete = 0;
    struct convert *cptr;
    extern int checknoretrieve (char *);
    int stat_ret;
    char realname[MAXPATHLEN];

    wu_realpath (name, realname, chroot_path);

#ifdef TRANSFER_COUNT
#ifdef TRANSFER_LIMIT
    if (retrieve_is_data)
        if ( ((file_limit_data_out   > 0) && (file_count_out   >= file_limit_data_out  ))
        ||   ((file_limit_data_total > 0) && (file_count_total >= file_limit_data_total))
        ||   ((data_limit_data_out   > 0) && (data_count_out   >= data_limit_data_out  ))
        ||   ((data_limit_data_total > 0) && (data_count_total >= data_limit_data_total)) ) {
            if (log_security)
                if (anonymous)
                    syslog (LOG_NOTICE, "anonymous(%s) of %s tried to retrieve %s (Transfer limits exceeded)",
                            guestpw, remoteident, realname);
                else
                    syslog (LOG_NOTICE, "%s of %s tried to retrieve %s (Transfer limits exceeded)",
                            pw->pw_name, remoteident, realname);
            reply(553, "Permission denied. (Transfer limits exceeded)");
            return;
        }
    if ( ((file_limit_raw_out   > 0) && (xfer_count_out   >= file_limit_raw_out  ))
    ||   ((file_limit_raw_total > 0) && (xfer_count_total >= file_limit_raw_total))
    ||   ((data_limit_raw_out   > 0) && (byte_count_out   >= data_limit_raw_out  ))
    ||   ((data_limit_raw_total > 0) && (byte_count_total >= data_limit_raw_total)) ) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to retrieve %s (Transfer limits exceeded)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to retrieve %s (Transfer limits exceeded)",
                        pw->pw_name, remoteident, realname);
        reply(553, "Permission denied. (Transfer limits exceeded)");
        return;
    }
#endif
#endif

    if (cmd == NULL && (stat_ret = stat(name, &st)) == 0)
        /* there isn't a command and the file exists */
	if (use_accessfile && checknoretrieve(name)) {	/* see above.  _H*/
            if(log_security) {
                if (anonymous) {
                    syslog(LOG_NOTICE, "anonymous(%s) of %s tried to download %s (noretrieve)",
                           guestpw, remoteident, realname);
                } else {
                    syslog(LOG_NOTICE, "%s of %s tried to download %s (noretrieve)",
                           pw->pw_name, remoteident, realname);
                }
            }
	    return;
	}
    logname = (char *)NULL;
    if (cmd == NULL && stat_ret != 0) { /* file does not exist */
         char *ptr;

        for (cptr = cvtptr; cptr != NULL; cptr = cptr->next) {
            if (!(mangleopts & O_COMPRESS) && (cptr->options & O_COMPRESS))
                continue;
            if (!(mangleopts & O_UNCOMPRESS) && (cptr->options & O_UNCOMPRESS))
                continue;
            if (!(mangleopts & O_TAR) && (cptr->options & O_TAR))
                continue;

            if ( (cptr->stripfix) && (cptr->postfix) ) {
                int pfxlen = strlen(cptr->postfix);
		int sfxlen = strlen(cptr->stripfix);
                int namelen = strlen(name);
                strcpy(fnbuf, name);

                if (namelen <= pfxlen)
                    continue;
		if ((namelen - pfxlen + sfxlen) >= sizeof(fnbuf))
		    continue;

		if (strcmp(fnbuf + namelen - pfxlen, cptr->postfix))
		    continue;
                *(fnbuf + namelen - pfxlen) = '\0';
                strcat(fnbuf, cptr->stripfix);
                if (stat(fnbuf, &st) != 0) 
                    continue;
            } else if (cptr->postfix) {
                int pfxlen = strlen(cptr->postfix);
                int namelen = strlen(name);

                if (namelen <= pfxlen)
                    continue;
                strcpy(fnbuf, name);
                if (strcmp(fnbuf + namelen - pfxlen, cptr->postfix))
                    continue;
                *(fnbuf + namelen - pfxlen) = (char) NULL;
                if (stat(fnbuf, &st) != 0)
                    continue;
            } else if (cptr->stripfix) {
                strcpy(fnbuf, name);
                strcat(fnbuf, cptr->stripfix);
                if (stat(fnbuf, &st) != 0)
                    continue;
            } else {
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                if (!cptr->types || !(cptr->types & T_DIR)) {
                    reply(550, "Cannot %s directories.", cptr->name);
                    return;
                }
                if (cptr->options && cptr->options & O_TAR) {
                    strcpy(namebuf, fnbuf);
                    strcat(namebuf, "/.notar");
                    if (stat(namebuf, &junk) == 0) {
                        if(log_security) {
                            if (anonymous) {
                                syslog(LOG_NOTICE, "anonymous(%s) of %s tried to tar %s (.notar)",
                                       guestpw, remoteident, realname);
                            } else {
                                syslog(LOG_NOTICE, "%s of %s tried to tar %s (.notar)",
                                       pw->pw_name, remoteident, name);
                            }
                        }
                        reply(550, "Sorry, you may not TAR that directory.");
                        return;
                    }
                }
            }
/* XXX: checknoretrieve() test is weak in that if I can't get /etc/passwd
   but I can tar /etc or /, I still win.  Be careful out there... _H*
   but you could put .notar in / and /etc and stop that ! */
	    if (use_accessfile && checknoretrieve(fnbuf)) {
                if (log_security)
                    if (anonymous)
                        syslog (LOG_NOTICE, "anonymous(%s) of %s tried to download %s (noretrieve)",
                                guestpw, remoteident, realname);
                    else
                        syslog (LOG_NOTICE, "%s of %s tried to download %s (noretrieve)",
                                pw->pw_name, remoteident, realname);
	        return;
	    }

            if (S_ISREG(st.st_mode) && ((cptr->types & T_REG) == 0)) {
                reply(550, "Cannot %s plain files.", cptr->name);
                return;
            }
            if (S_ISREG(st.st_mode) != 0 && S_ISDIR(st.st_mode) != 0) {
                reply(550, "Cannot %s special files.", cptr->name);
                return;
            }
            if (!(cptr->types & T_ASCII) && deny_badasciixfer(550, ""))
                return;

            logname = &fnbuf[0];
            options |= cptr->options;

            strcpy(namebuf, cptr->external_cmd);
            if ((ptr = strchr(namebuf, ' ')) != NULL)
                *ptr = '\0';
            if (stat(namebuf, &st) != 0) {
                syslog(LOG_ERR, "external command %s not found",
                       namebuf);
                reply(550,
                "Local error: conversion program not found. Cannot %s file.",
                             cptr->name);
                return;
            }
            retrieve(cptr->external_cmd, logname);

            goto dolog;  /* transfer of converted file completed */
        }
    } 

    if (cmd == NULL) { /* no command */
        fin = fopen(name, "r"), closefunc = fclose;
        st.st_size = 0;
    } else {           /* run command */
        static char line[BUFSIZ];

        snprintf(line, sizeof line, cmd, name), name = line;
        fin = ftpd_popen(line, "r", 1), closefunc = ftpd_pclose;
        st.st_size = -1;
#ifdef HAVE_ST_BLKSIZE
        st.st_blksize = BUFSIZ;
#endif
    }
    if (fin == NULL) {
        if (errno != 0)
            perror_reply(550, name);
        if ((errno == EACCES) || (errno == EPERM)) {
            if(log_security) {
                if (anonymous) {
                    syslog(LOG_NOTICE, "anonymous(%s) of %s tried to download %s (file permissions)",
                           guestpw, remoteident, realname);
                } else {
                    syslog(LOG_NOTICE, "%s of %s tried to download %s (file permissions)",
                           pw->pw_name, remoteident, realname);
                }
            }
        }
        return;
    }
    if (cmd == NULL &&
        (fstat(fileno(fin), &st) < 0 || (st.st_mode & S_IFMT) != S_IFREG)) {
        reply(550, "%s: not a plain file.", name);
        goto done;
    }
    if (restart_point) {
        if (type == TYPE_A) {
            register int i,
              n,
              c;

            n = restart_point;
            i = 0;
            while (i++ < n) {
                if ((c = getc(fin)) == EOF) {
                    perror_reply(550, name);
                    goto done;
                }
                if (c == '\n')
                    i++;
            }
        } else if (lseek(fileno(fin), restart_point, L_SET) < 0) {
            perror_reply(550, name);
            goto done;
        }
    }

#ifdef RATIO
     if( freefile = is_downloadfree(name) ) {
	 if( limit_download > 0 
	    && (total_free_dl+st.st_size)>limit_download) {
	     reply( 550, "%s: file size %d exceed download limit left %d.",
		   name, st.st_size, limit_download-total_free_dl );
	     goto done;
	 }
	 syslog(LOG_INFO, "%s is download free.", name );
     }
     else {
	 if(  cmd == NULL ) {
	     if( upload_download_rate > 0 ) {
		 size_t credit = total_upload*upload_download_rate-total_download;
		 if( st.st_size > credit  ) {
		     reply( 550, "%s: file size %d exceed credit %d.",
		       name, st.st_size, credit );
		     goto done;
		 }
	     }
	 }
     }
#endif /* RATIO */

    dout = dataconn(name, st.st_size, "w");
    if (dout == NULL)
        goto done;
#ifdef BUFFER_SIZE
#ifdef THROUGHPUT
    TransferComplete = send_data(name, fin, dout, BUFFER_SIZE);
#else
    TransferComplete = send_data(fin, dout, BUFFER_SIZE);
#endif
    #else
#ifdef HAVE_ST_BLKSIZE
#ifdef THROUGHPUT
    TransferComplete = send_data(name, fin, dout, st.st_blksize*2);
#else
    TransferComplete = send_data(fin, dout, st.st_blksize*2);
#endif
#else
#ifdef THROUGHPUT
    TransferComplete = send_data(name, fin, dout, BUFSIZ);
#else
    TransferComplete = send_data(fin, dout, BUFSIZ);
#endif
#endif
#endif /* BUFFER_SIZE */
#ifdef KRB5
    secure_flush(fileno(dout));
#endif
    fclose(dout);

  dolog:
    if (ThisRetrieveIsData)
        fb_realpath ((logname != NULL) ? logname : name, LastFileTransferred);

    if (log_outbound_xfers && (xferlog || syslogmsg) && (cmd == 0)) {
        char msg[MAXPATHLEN+256]; /* 256 = 2*64 (max. hostname length) + 24 + 2 * strlen(MAXINT) */
	char *msg2;		/* for stupid_sprintf */
        int xfertime = time(NULL) - start_time;
        time_t curtime = time(NULL);
        int loop;

        if (!xfertime)
            xfertime++;
#ifdef XFERLOG_REALPATH
        wu_realpath((logname != NULL) ? logname : name, &namebuf[0], chroot_path); 
#else
	fb_realpath((logname != NULL) ? logname : name, &namebuf[0]);
#endif
        for (loop = 0; namebuf[loop]; loop++)
            if (isspace(namebuf[loop]) || iscntrl(namebuf[loop]))
                namebuf[loop] = '_';

/* Some sprintfs can't deal with a lot of arguments, so we split this */
        sprintf(msg, "%.24s %d %s " PRINTF_QD " ",
                ctime(&curtime),
                xfertime,
                remotehost,
                byte_count
	    );
	msg2 = msg + strlen(msg);	/* sigh */
        sprintf(msg2, "%s %c %s %c %c %s ftp %d %s %c\n",
                namebuf,
                (type == TYPE_A) ? 'a' : 'b',
                opt_string(options),
                'o',
                anonymous ? 'a' : (guest ? 'g' : 'r'),
                anonymous ? guestpw : pw->pw_name,
                authenticated,
                authenticated ? authuser : "*",
                TransferComplete ? 'c' : 'i'
            );
      if (!syslogmsg)
        write(xferlog, msg, strlen(msg));
      else
        syslog(LOG_INFO, "xferlog (send): %s", msg+25);
    }
    data = -1;
    pdata = -1;
  done:
    if (closefunc)
        (*closefunc) (fin);
}

void store(char *name, char *mode, int unique)
{
    FILE *fout, *din;
    struct stat st;
    int (*closefunc) ();
    int TransferIncomplete = 1;
    char *gunique(char *local);
    time_t start_time = time(NULL);

    struct aclmember *entry = NULL;

    int fdout;
    char realname[MAXPATHLEN];

#ifdef OVERWRITE
    int overwrite = 1;
#endif /* OVERWRITE */

    int open_flags=0;

#ifdef UPLOAD
    mode_t oldmask;
    uid_t uid;
    gid_t gid;
    uid_t oldid;
    int f_mode = -1,
      dir_mode,
      match_value = -1;
    int valid = 0;
    open_flags = (O_RDWR | O_CREAT |
		      ((mode != NULL && *mode == 'a') ? O_APPEND : O_TRUNC));
#endif /* UPLOAD */

    wu_realpath (name, realname, chroot_path);

#ifdef TRANSFER_COUNT
#ifdef TRANSFER_LIMIT
    if ( ((file_limit_data_in    > 0) && (file_count_in    >= file_limit_data_in   ))
    ||   ((file_limit_data_total > 0) && (file_count_total >= file_limit_data_total))
    ||   ((data_limit_data_in    > 0) && (data_count_in    >= data_limit_data_in   ))
    ||   ((data_limit_data_total > 0) && (data_count_total >= data_limit_data_total)) ) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload %s (Transfer limits exceeded)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload %s (Transfer limits exceeded)",
                        pw->pw_name, remoteident, realname);
        reply(553, "Permission denied. (Transfer limits exceeded)");
        return;
    }
    if ( ((file_limit_raw_in    > 0) && (xfer_count_in    >= file_limit_raw_in   ))
    ||   ((file_limit_raw_total > 0) && (xfer_count_total >= file_limit_raw_total))
    ||   ((data_limit_raw_in    > 0) && (byte_count_in    >= data_limit_raw_in   ))
    ||   ((data_limit_raw_total > 0) && (byte_count_total >= data_limit_raw_total)) ) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload %s (Transfer limits exceeded)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload %s (Transfer limits exceeded)",
                        pw->pw_name, remoteident, realname);
        reply(553, "Permission denied. (Transfer limits exceeded)");
        return;
    }
#endif
#endif

    if (unique && stat(name, &st) == 0 &&
        (name = gunique(name)) == NULL)
        return;

    /*
     * check the filename, is it legal?
     */
    if ( (fn_check(name)) <= 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload \"%s\" (bad filename)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload \"%s\" (bad filename)",
                        pw->pw_name, remoteident, realname);
        return;
    }

#ifdef OVERWRITE
    /* if overwrite permission denied and file exists... then deny the user
     * permission to write the file. */
    while (getaclentry("overwrite", &entry) && ARG0 && ARG1 != NULL) {
        if (type_match(ARG1))
            if (strcasecmp(ARG0, "yes") != 0) {
                overwrite = 0;
                open_flags |= O_EXCL;
            }
    }

    if (!overwrite && !stat(name, &st)) {
        if(log_security) {
            if (anonymous) {
                syslog(LOG_NOTICE, "anonymous(%s) of %s tried to overwrite %s",
                       guestpw, remoteident, realname);
            } else {
                syslog(LOG_NOTICE, "%s of %s tried to overwrite %s",
                       pw->pw_name, remoteident, realname);
            }
        }
        reply(553, "%s: Permission denied. (Overwrite)", name);
        return;
    }
#endif /* OVERWRITE */

#ifdef RATIO
    if( limit_upload > 0 && total_upload >= limit_upload ) {
	reply(553, "Upload limit exceed. limit=%d  uploaded=%d.",
	      limit_upload, total_upload );
	return;
    }
#endif /* RATIO */

#ifdef UPLOAD
    if ( (match_value = upl_check(name, &uid, &gid, &f_mode, &valid)) < 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload %s (upload denied)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload %s (upload denied)",
                        pw->pw_name, remoteident, realname);
        return;
    }

    /* do not truncate the file if we are restarting */
    if (restart_point)
        open_flags &= ~O_TRUNC;

    /* if the user has an explicit new file mode, than open the file using
     * that mode.  We must take care to not let the umask affect the file
     * mode.
     * 
     * else open the file and let the default umask determine the file mode. */
    if (f_mode >= 0) {
        oldmask = umask(0000);
        fdout = open(name, open_flags, f_mode);
        umask(oldmask);
    } else
        fdout = open(name, open_flags, 0666);

    if (fdout < 0) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload %s (permissions)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload %s (permissions)",
                        pw->pw_name, remoteident, realname);
        perror_reply(553, name);
        return;
    }
    /* if we have a uid and gid, then use them. */

    if (valid > 0) {
        oldid = geteuid();
        delay_signaling(); /* we can't allow any signals while euid==0: kinch */
        seteuid((uid_t) 0);
        if ((fchown(fdout, uid, gid)) < 0) {
            seteuid(oldid);
            enable_signaling(); /* we can allow signals once again: kinch */
            perror_reply(550, "fchown");
            return;
        }
        seteuid(oldid);
        enable_signaling(); /* we can allow signals once again: kinch */
    }
#endif /* UPLOAD */

    if (restart_point && (open_flags & O_APPEND) == 0)
        mode = "r+";

#ifdef UPLOAD
    fout = fdopen(fdout, mode);
#else
    fout = fopen(name, mode);
#endif /* UPLOAD */

    closefunc = fclose;
    if (fout == NULL) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to upload %s (permissions)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to upload %s (permissions)",
                        pw->pw_name, remoteident, realname);
        perror_reply(553, name);
        return;
    }
    if (restart_point) {
        if (type == TYPE_A) {
            register int i,
              n,
              c;

            n = restart_point;
            i = 0;
            while (i++ < n) {
                if ((c = getc(fout)) == EOF) {
                    perror_reply(550, name);
                    goto done;
                }
                if (c == '\n')
                    i++;
            }
            /* We must do this seek to "current" position because we are
             * changing from reading to writing. */
            if (fseek(fout, 0L, L_INCR) < 0) {
                perror_reply(550, name);
                goto done;
            }
        } else if (lseek(fileno(fout), restart_point, L_SET) < 0) {
            perror_reply(550, name);
            goto done;
        }
    }
    din = dataconn(name, (off_t) -1, "r");
    if (din == NULL)
        goto done;
    if ((TransferIncomplete = receive_data(din, fout)) == 0) {
#ifdef RATIO
	total_upload += byte_count;
	show_message(226,C_WD);
#endif /* RATIO */
        if (unique)
            reply(226, "Transfer complete (unique file name:%s).",
                  name);
        else
            reply(226, "Transfer complete.");
    }
    fclose(din);

  dolog:
    fb_realpath(name, LastFileTransferred);
    
#ifdef MAIL_ADMIN
    if (anonymous && incmails > 0) { 
        FILE *sck = NULL;
        struct aclmember *entry = NULL;
        unsigned char temp = 0, temp2 = 0;
        char pathname[MAXPATHLEN];
    	
        while ((temp < mailservers) && (sck == NULL)) {
            sck = SockOpen(mailserver[temp++],25);
        }
        if (sck == NULL) {
            syslog(LOG_ERR, "Can't connect to a mailserver.");
            goto mailfail;
        }
        if (Reply(sck) != 220) {
            syslog(LOG_ERR, "Mailserver failed to initiate contact.");
            goto mailfail;
        }
        if (Send(sck, "HELO localhost\r\n") != 250) {
            syslog(LOG_ERR, "Mailserver doesn't understand HELO.");
            goto mailfail;
        }
        if (Send(sck, "MAIL FROM: <%s>\r\n", email(mailfrom)) != 250) {
            syslog(LOG_ERR, "Mailserver didn't accept MAIL FROM.");
            goto mailfail;
        }
        for (temp = 0; temp < incmails; temp++) {
            if (Send(sck, "RCPT TO: <%s>\r\n", email(incmail[temp])) == 250)
                temp2++;
        }
        if (temp2 == 0) {
            syslog(LOG_ERR, "Mailserver didn't accept any RCPT TO.");
            goto mailfail;
        }
        if (Send(sck, "DATA\r\n") != 354) {
            syslog(LOG_ERR, "Mailserver didn't accept DATA.");
            goto mailfail;
        }
        SockPrintf(sck, "From: Bero FTPD <%s>\r\n", mailfrom);
        SockPrintf(sck, "Subject: New file uploaded: %s\r\n\r\n", name);
        fb_realpath(name, pathname);
        SockPrintf(sck, "%s uploaded %s from %s.\r\nFile size is %d.\r\nPlease move the file where is belongs.\r\n", guestpw, pathname, remotehost, byte_count);
        if (Send(sck, ".\r\n") != 250)
            syslog(LOG_ERR, "Message rejected by mailserver.");
        if (Send(sck, "QUIT\r\n") != 221)
            syslog(LOG_ERR, "Mailserver didn't accept QUIT.");
        goto mailok;
      mailfail:
        if (sck != NULL)
            fclose(sck);
      mailok:
	sck=NULL; /* We don't need this, but some compilers need an
		     instruction after a label. This one can't hurt. */
    }
#endif
    if (log_incoming_xfers && (xferlog || syslogmsg)) {
        char namebuf[MAXPATHLEN],
          msg[MAXPATHLEN];
	char *msg2;		/* for stupid_sprintf */
        int xfertime = time(NULL) - start_time;
        time_t curtime = time(NULL);
        int loop;

        if (!xfertime)
            xfertime++;
#ifdef XFERLOG_REALPATH
        wu_realpath(name, namebuf, chroot_path);
#else
	fb_realpath(name, namebuf);
#endif
        for (loop = 0; namebuf[loop]; loop++)
            if (isspace(namebuf[loop]) || iscntrl(namebuf[loop]))
                namebuf[loop] = '_';

        sprintf(msg, "%.24s %d %s " PRINTF_QD " ",
                ctime(&curtime),
                xfertime,
                remotehost,
                byte_count
	    );
	msg2 = msg + strlen(msg);	/* sigh */
        sprintf(msg2, "%s %c %s %c %c %s ftp %d %s %c\n",
                namebuf,
                (type == TYPE_A) ? 'a' : 'b',
                opt_string(0),
                'i',
                anonymous ? 'a' : (guest ? 'g' : 'r'),
                anonymous ? guestpw : pw->pw_name,
                authenticated,
                authenticated ? authuser : "*",
                TransferIncomplete ? 'i' : 'c'
            );
      if (!syslogmsg)
        write(xferlog, msg, strlen(msg));
      else
        syslog(LOG_INFO, "xferlog (recv): %s", msg+25);
    }
    data = -1;
    pdata = -1;
  done:
    (*closefunc) (fout);
}

FILE * getdatasock(char *mode)
{
      int s,
      on = 1,
      tries;

    if (data >= 0)
        return (fdopen(data, mode));
    delay_signaling(); /* we can't allow any signals while euid==0: kinch */
    seteuid((uid_t) 0);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        goto bad;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &on, sizeof(on)) < 0)
        goto bad;
    if (TCPwindowsize)
        setsockopt(s, SOL_SOCKET, (*mode == 'w' ? SO_SNDBUF : SO_RCVBUF),
                          (char *) &TCPwindowsize, sizeof(TCPwindowsize));
    /* anchor socket to avoid multi-homing problems */
    data_source.sin_family = AF_INET;
    data_source.sin_addr = ctrl_addr.sin_addr;

#if defined(VIRTUAL) && defined(CANT_BIND) /* can't bind to virtual address */
    data_source.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    for (tries = 1;; tries++) {
        if (bind(s, (struct sockaddr *) &data_source,
                 sizeof(data_source)) >= 0)
            break;
        if (errno != EADDRINUSE || tries > 10)
            goto bad;
        sleep(tries);
    }
#if defined(M_UNIX) && !defined(_M_UNIX)  /* bug in old TCP/IP release */
    {
        struct linger li;
        li.l_onoff = 1;
        li.l_linger = 900;
        if (setsockopt(s, SOL_SOCKET, SO_LINGER,
          (char *)&li, sizeof(struct linger)) < 0) {
            syslog(LOG_WARNING, "setsockopt (SO_LINGER): %m");
            goto bad;
        }
    }
#endif
    seteuid((uid_t) pw->pw_uid);
    enable_signaling(); /* we can allow signals once again: kinch */

#ifdef IPTOS_THROUGHPUT
    on = IPTOS_THROUGHPUT;
    if (setsockopt(s, IPPROTO_IP, IP_TOS, (char *) &on, sizeof(int)) < 0)
          syslog(LOG_WARNING, "setsockopt (IP_TOS): %m");
#endif
#ifdef TCP_NOPUSH
	/*
	 * Turn off push flag to keep sender TCP from sending short packets
	 * at the boundaries of each write().  Should probably do a SO_SNDBUF
	 * to set the send buffer size as well, but that may not be desirable
	 * in heavy-load situations.
	 */
	on = 1;
	if (setsockopt(s, IPPROTO_TCP, TCP_NOPUSH, (char *)&on, sizeof on) < 0)
		syslog(LOG_WARNING, "setsockopt (TCP_NOPUSH): %m");
#endif

    return (fdopen(s, mode));
  bad:
    on = errno; /* hold errno for return */
    seteuid((uid_t) pw->pw_uid);
    enable_signaling(); /* we can allow signals once again: kinch */
    if (s != -1)
        close(s);
    errno = on; 
    return (NULL);
}

FILE * dataconn(char *name, off_t size, char *mode)
{
    char sizebuf[32];
    FILE *file;
    int retry = 0;
#ifdef IPTOS_LOWDELAY
    int tos;
#endif
#ifdef THROUGHPUT
    int bps;
    double bpsmult;
#endif

    file_size = size;
    byte_count = 0;
    if (size != (off_t) - 1)
        sprintf(sizebuf, " (" PRINTF_QD " bytes)", size);
    else
        strcpy(sizebuf, "");
    if (pdata >= 0) {
        struct sockaddr_in from;
        char dataaddr[MAXHOSTNAMELEN];
        size_t fromlen = sizeof(from);
        int s;
#ifdef FD_ZERO
        struct timeval timeout;
        fd_set set;
 
        if (TCPwindowsize)
            setsockopt(pdata, SOL_SOCKET, (*mode == 'w' ? SO_SNDBUF : SO_RCVBUF),
                              (char *) &TCPwindowsize, sizeof(TCPwindowsize));
        FD_ZERO(&set);
        FD_SET(pdata, &set);
 
        timeout.tv_usec = 0;
        timeout.tv_sec = 120;
#ifdef HPUX_SELECT
        if (select(pdata+1, (int *)&set, NULL, NULL, &timeout) == 0 ||
#else
        if (select(pdata+1, &set, (fd_set *) 0, (fd_set *) 0, 
		   (struct timeval *) &timeout) == 0 ||
#endif
            (s = accept(pdata, (struct sockaddr *) &from, &fromlen)) < 0) {
#else      
        if (TCPwindowsize)
            setsockopt(pdata, SOL_SOCKET, (*mode == 'w' ? SO_SNDBUF : SO_RCVBUF),
                              (char *) &TCPwindowsize, sizeof(TCPwindowsize));
        alarm(120);
        s = accept(pdata, (struct sockaddr *) &from, &fromlen);
        alarm(0);
        if (s < 0) {
#endif
            reply(425, "Can't open data connection.");
            close(pdata);
            pdata = -1;
            return (NULL);
        }
        close(pdata);
        pdata = s;
#ifdef IPTOS_LOWDELAY
        tos = IPTOS_LOWDELAY;
        setsockopt(s, IPPROTO_IP, IP_TOS, (char *) &tos,
                          sizeof(int));

#endif
        strncpy(dataaddr, inet_ntoa(from.sin_addr), sizeof(dataaddr));
        if (strcasecmp(dataaddr, remoteaddr) != 0) { /***DEBUG: CASE***/
        /* 
         * This will log when data connection comes from an address different
         * than the control connection.
         */
            syslog(LOG_NOTICE, "%s of %s: data connect from %s for %s%s",
                anonymous ? guestpw : pw->pw_name, remoteident,
                dataaddr, name, sizebuf);
        }
#ifdef THROUGHPUT
        throughput_calc(name, &bps, &bpsmult);
        if (bps != -1) {
            lreply(150, "Opening %s mode data connection for %s%s.",
                   type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
            reply(150, "Restricting network throughput to %d bytes/s.", bps);
        }
        else
#endif
        reply(150, "Opening %s mode data connection for %s%s.",
              type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
        return (fdopen(pdata, mode));
    }
    if (data >= 0) {
        reply(125, "Using existing data connection for %s%s.",
              name, sizebuf);
        usedefault = 1;
        return (fdopen(data, mode));
    }
    if (usedefault)
        data_dest = his_addr;
    if (data_dest.sin_port == 0) {
      reply(500, "Can't build data connection: no PORT specified");
      return (NULL);
    }
    usedefault = 1;
    file = getdatasock(mode);
    if (file == NULL) {
        reply(425, "Can't create data socket (%s,%d): %s.",
              inet_ntoa(data_source.sin_addr),
              ntohs(data_source.sin_port), strerror(errno));
        return (NULL);
    }
    data = fileno(file);
    while (connect(data, (struct sockaddr *) &data_dest,
                   sizeof(data_dest)) < 0) {
        if ((errno == EADDRINUSE || errno == EINTR) && retry < swaitmax) {
            sleep((unsigned) swaitint);
            retry += swaitint;
            continue;
        }
        perror_reply(425, "Can't build data connection");
        fclose(file);
        data = -1;
        return (NULL);
    }
    if (TCPwindowsize)
        setsockopt(data, SOL_SOCKET, (*mode == 'w' ? SO_SNDBUF : SO_RCVBUF),
                          (char *) &TCPwindowsize, sizeof(TCPwindowsize));
#ifdef THROUGHPUT
    throughput_calc(name, &bps, &bpsmult);
    if (bps != -1) {
        lreply(150, "Opening %s mode data connection for %s%s.",
               type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
        reply(150, "Restricting network throughput to %d bytes/s.", bps);
    }
    else 
#endif
    reply(150, "Opening %s mode data connection for %s%s.",
          type == TYPE_A ? "ASCII" : "BINARY", name, sizebuf);
    return (file);
}

/* Tranfer the contents of "instr" to "outstr" peer using the appropriate
 * encapsulation of the data subject to Mode, Structure, and Type.
 *
 * NB: Form isn't handled. */

#ifdef THROUGHPUT
int send_data(char *name, FILE *instr, FILE *outstr, off_t blksize)
#else
int send_data(FILE *instr, FILE *outstr, off_t blksize)
#endif
{
    register int c,
      cnt;
    static char *buf;
    int netfd,
      filefd;
    size_t size_t_blksize;
#ifdef THROUGHPUT
    int bps;
    double bpsmult;
    time_t t1, t2;
#endif

#ifdef THROUGHPUT
    throughput_calc(name, &bps, &bpsmult);
#endif

    buf = NULL;
    if (setjmp(urgcatch)) {
        alarm(0);
        transflag = 0;
        if (buf)
            free(buf);
        retrieve_is_data = 1;
        return (0);
    }
    transflag++;
    switch (type) {

    case TYPE_A:
        alarm((unsigned)timeout);
        while ((c = getc(instr)) != EOF) {
            if (++byte_count % 4096 == 0)
               alarm((unsigned)timeout);
            if (c == '\n') {
                if (ferror(outstr))
                    goto data_err;
#ifdef KRB5
                secure_putc('\r', outstr);
#else
		fputc('\r', outstr);
#endif
#ifdef TRANSFER_COUNT
                if (retrieve_is_data) {
                    data_count_total++;
                    data_count_out++;
                }
                byte_count_total++;
                byte_count_out++;
#endif
            }
#ifdef KRB5
            secure_putc(c, outstr);
#else
	    fputc(c, outstr);
#endif
#ifdef TRANSFER_COUNT
            if (retrieve_is_data) {
                data_count_total++;
                data_count_out++;
            }
            byte_count_total++;
            byte_count_out++;
#endif
        }
	alarm(0);
#ifdef KRB5
        secure_flush(outstr);
#else
	fflush(outstr);
#endif
        transflag = 0;
        if (ferror(instr))
            goto file_err;
        if (ferror(outstr))
            goto data_err;
#ifdef RATIO
	if( freefile ) {
	    total_free_dl += byte_count;
	}
	else {
	    total_download += byte_count;
	}
	show_message( 226, C_WD );
#endif /* RATIO */
        reply(226, "Transfer complete.");
#ifdef TRANSFER_COUNT
        if (retrieve_is_data) {
            file_count_total++;
            file_count_out++;
        }
        xfer_count_total++;
        xfer_count_out++;
#endif
	retrieve_is_data = 1;
        return (1);

    case TYPE_I:
    case TYPE_L:
#ifdef THROUGHPUT
        if (bps != -1)
            blksize = bps;
#endif
        size_t_blksize = blksize;
        if ((buf = (char *) malloc(size_t_blksize)) == NULL) {
            transflag = 0;
            perror_reply(451, "Local resource failure: malloc");
            retrieve_is_data = 1;
            return (0);
        }
        netfd = fileno(outstr);
        filefd = fileno(instr);
	alarm((unsigned)timeout);
#ifdef THROUGHPUT
        if (bps != -1)
            t1 = time(NULL);
#endif
        while ((cnt = read(filefd, buf, size_t_blksize)) > 0 &&
#ifdef KRB5
               secure_write(netfd, buf, cnt) == cnt){
#else
               write(netfd, buf, cnt) == cnt){
#endif
	  alarm((unsigned)timeout);
	  byte_count += cnt;
#ifdef TRANSFER_COUNT
          if (retrieve_is_data) {
              data_count_total += cnt;
              data_count_out += cnt;
          }
          byte_count_total += cnt;
          byte_count_out += cnt;
#endif
#ifdef THROUGHPUT
          if (bps != -1) {
              t2 = time(NULL);
              if (t2 == t1)
                  sleep(1);
              t1 = time(NULL);
          }
#endif
	}
	alarm(0);
#ifdef KRB5
	secure_flush(netfd);
#endif
#ifdef THROUGHPUT
        if (bps != -1)
            throughput_adjust(name);
#endif
        transflag = 0;
        free(buf);
        if (cnt != 0) {
            if (cnt < 0)
                goto file_err;
            goto data_err;
        }
#ifdef TRANSFER_COUNT
        if (retrieve_is_data) {
            file_count_total++;
            file_count_out++;
        }
        xfer_count_total++;
        xfer_count_out++;
#endif
#ifdef RATIO
	if( freefile ) {
	    total_free_dl += byte_count;
	}
	else {
	    total_download += byte_count;
	}
	show_message(226,C_WD);
#endif /* RATIO */
        reply(226, "Transfer complete.");
	retrieve_is_data = 1;
        return (1);
    default:
        transflag = 0;
        reply(550, "Unimplemented TYPE %d in send_data", type);
#ifdef TRANSFER_COUNT
	retrieve_is_data = 1;
#endif
        return (0);
   }


  data_err:
    alarm(0);
    transflag = 0;
    perror_reply(426, "Data connection");
    retrieve_is_data = 1;
    return (0);

  file_err:
    transflag = 0;
    perror_reply(551, "Error on input file");
    retrieve_is_data = 1;
    return (0);
}

/* Transfer data from peer to "outstr" using the appropriate encapulation of
 * the data subject to Mode, Structure, and Type.
 *
 * N.B.: Form isn't handled. */

int receive_data(FILE *instr, FILE *outstr)
{
    register int c;
    int cnt,
      bare_lfs = 0;
    static char *buf;
    int netfd,
      filefd;
#ifdef BUFFER_SIZE
    size_t buffer_size = BUFFER_SIZE;
#else
    size_t buffer_size = BUFSIZ;
#endif

    buf = NULL;
    if (setjmp(urgcatch)) {
        transflag = 0;
        if (buf)
            free(buf);
        return (-1);
    }
    transflag++;
    switch (type) {

    case TYPE_I:
    case TYPE_L:
        if ((buf = (char *) malloc(buffer_size)) == NULL) {
            transflag = 0;
            perror_reply(451, "Local resource failure: malloc");
            return (-1);
        }
        netfd = fileno(instr);
        filefd = fileno(outstr);
	alarm((unsigned)timeout);
#ifdef KRB5
        while ((cnt = secure_read(netfd, buf, buffer_size)) > 0 &&
#else
        while ((cnt = read(netfd, buf, buffer_size)) > 0 &&
#endif
               write(filefd, buf, cnt) == cnt){
            byte_count += cnt;
#ifdef TRANSFER_COUNT
            data_count_total += cnt;
            data_count_in += cnt;
            byte_count_total += cnt;
            byte_count_in += cnt;
#endif
	    alarm((unsigned)timeout);
	}
	alarm(0);
        transflag = 0;
        free(buf);
        if (cnt != 0) {
            if (cnt < 0)
                goto data_err;
            goto file_err;
        }
#ifdef TRANSFER_COUNT
        file_count_total++;
        file_count_in++;
        xfer_count_total++;
        xfer_count_in++;
#endif
        return (0);

    case TYPE_E:
        reply(553, "TYPE E not implemented.");
        transflag = 0;
        return (-1);

    case TYPE_A:
	alarm((unsigned)timeout);
#ifdef KRB5
        while ((c = secure_getc(instr)) != EOF) {
#else
        while ((c = getc(instr)) != EOF) {
#endif
            if (++byte_count % 4096 == 0)
               alarm((unsigned)timeout);
            if (c == '\n')
                bare_lfs++;
            while (c == '\r') {
                if (ferror(outstr))
                    goto file_err;
		alarm((unsigned)timeout);
#ifdef KRB5
                if ((c = secure_getc(instr)) != '\n') {
#else
                if ((c = getc(instr)) != '\n') {
#endif
                    putc('\r', outstr);
#ifdef TRANSFER_COUNT
                    data_count_total++;
                    data_count_in++;
                    byte_count_total++;
                    byte_count_in++;
#endif
                    if (c == EOF) /* null byte fix, noid@cyborg.larc.nasa.gov */
                        goto contin2;
	            if (++byte_count % 4096 == 0)
	               alarm((unsigned)timeout);
                }
            }
            putc(c, outstr);
#ifdef TRANSFER_COUNT
            data_count_total++;
            data_count_in++;
            byte_count_total++;
            byte_count_in++;
#endif
          contin2:;
        }
	alarm(0);
        fflush(outstr);
        if (ferror(instr))
            goto data_err;
        if (ferror(outstr))
            goto file_err;
        transflag = 0;
        if (bare_lfs) {
            lreply(226, "WARNING! %d bare linefeeds received in ASCII mode", bare_lfs);
            lreply(0, "   File may not have transferred correctly.");
        }
#ifdef TRANSFER_COUNT
        file_count_total++;
        file_count_in++;
        xfer_count_total++;
        xfer_count_in++;
#endif
        return (0);
    default:
        reply(550, "Unimplemented TYPE %d in receive_data", type);
        transflag = 0;
        return (-1);
    }

  data_err:
    alarm(0);
    transflag = 0;
    perror_reply(426, "Data Connection");
    return (-1);

  file_err:
    transflag = 0;
    perror_reply(452, "Error writing file");
    return (-1);
}

void statfilecmd(char *filename)
{
#ifndef INTERNAL_LS
    char line[BUFSIZ], *ptr;
    FILE *fin;
    int c;
    if (anonymous && dolreplies)
        snprintf(line, sizeof(line), ls_long, filename);
    else
        snprintf(line, sizeof(line), ls_short, filename);
    fin = ftpd_popen(line, "r", 0);
#endif
    lreply(213, "status of %s:", filename);
#ifndef INTERNAL_LS
    /*
    while ((c = getc(fin)) != EOF) {
        if (c == '\n') {
            if (ferror(stdout)) {
                perror_reply(421, "control connection");
                ftpd_pclose(fin);
                dologout(1);
                / * NOTREACHED * /
            }
            if (ferror(fin)) {
                perror_reply(551, filename);
                ftpd_pclose(fin);
                return;
            }
            putc('\r', stdout);
        }
        putc(c, stdout);
    }
    */
    while (fgets(line, sizeof(line), fin) != NULL) {
        if ((ptr = strchr(line, '\n'))) /* clip out unnecessary newline */
            *ptr = '\0';
	lreply(0, "%s", line);
    }
    ftpd_pclose(fin);
#else
    ls_dir(filename,1,0,1,0,1,stdout);
#endif
    reply(213, "End of Status");
}

void statcmd()
{
    struct sockaddr_in *sin;
    u_char *a,
     *p;

    lreply(211, "%s FTP server status:", hostname);
    lreply(0, "     %s", version);
    if (!isdigit(remotehost[0]))
        lreply(0, "     Connected to %s (%s)", remotehost,
	       inet_ntoa(his_addr.sin_addr));
    else
        lreply(0, "     Connected to %s", remotehost);
		
#ifdef KRB5
    if (auth_type) {
		reply(0, "     Authentication type: %s", auth_type);
    }
#endif

    if (logged_in) {
        if (anonymous)
            lreply(0, "     Logged in anonymously");
        else
            lreply(0, "     Logged in as %s", pw->pw_name);
    } else if (askpasswd)
        lreply(0, "     Waiting for password");
    else
        lreply(0, "     Waiting for user name");

#ifdef KRB5
    reply(0, "     PROTection level: %s", prot_levelnames[prot_level]);
#endif

    if (type == TYPE_L)
        lreply(0, "     TYPE: %s %d; STRUcture: %s; transfer MODE: %s",
	       typenames[type], NBBY, strunames[stru], modenames[mode]);
    else
        lreply(0, "     TYPE: %s%s%s; STRUcture: %s; transfer MODE: %s",
	       typenames[type], (type == TYPE_A || type == TYPE_E) ?
	       ", FORM: " : "", (type == TYPE_A || type == TYPE_E) ?
	       formnames[form] : "", strunames[stru], modenames[mode]);
    if (data != -1)
        lreply(0, "     Data connection open");
    else if (pdata != -1 || usedefault == 0){
      sin = ( usedefault == 0 ? &data_dest : &pasv_addr );
      a = (u_char *) & sin->sin_addr;
      p = (u_char *) & sin->sin_port;
#define UC(b) (((int) b) & 0xff)
      lreply(0, "     %s (%d,%d,%d,%d,%d,%d)",
	     usedefault == 0 ? "PORT" : "in Passive mode",
	     UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));
#undef UC
    }
    else
        lreply(0, "     No data connection");
#ifdef TRANSFER_COUNT
    lreply(0, "     %d data bytes received in %d files", data_count_in, file_count_in);
    lreply(0, "     %d data bytes transmitted in %d files", data_count_out, file_count_out);
    lreply(0, "     %d data bytes total in %d files", data_count_total, file_count_total);
    lreply(0, "     %d traffic bytes received in %d transfers", byte_count_in, xfer_count_in);
    lreply(0, "     %d traffic bytes transmitted in %d transfers", byte_count_out, xfer_count_out);
    lreply(0, "     %d traffic bytes total in %d transfers", byte_count_total, xfer_count_total);
#endif
    reply(211, "End of status");
}

void fatal(char *s)
{
    reply(451, "Error in server: %s\n", s);
    reply(221, "Closing connection due to server error.");
    dologout(0);
    /* NOTREACHED */
}

#define USE_REPLY_NOTFMT	(1<<1)	/* fmt is not a printf fmt (KLUDGE) */
#define USE_REPLY_LONG		(1<<2)	/* this is a long reply; use a - */

void vreply(long flags, int n, char *fmt, va_list ap)
{
  char buf[FTP_BUFSIZ];

  flags &= USE_REPLY_NOTFMT | USE_REPLY_LONG;

  if (n) /* if numeric is 0, don't output one; use n==0 in place of printf's */
    sprintf(buf, "%03d%c", n, flags & USE_REPLY_LONG ? '-' : ' ');

  /* This is somewhat of a kludge for autospout.  I personally think that
   * autospout should be done differently, but that's not my department. -Kev
   */
  if (flags & USE_REPLY_NOTFMT)
    snprintf(buf + (n ? 4 : 0), n ? sizeof(buf) - 4 : sizeof(buf), "%s", fmt);
  else
    vsnprintf(buf + (n ? 4 : 0), n ? sizeof(buf) - 4 : sizeof(buf), fmt, ap);

#ifdef KRB5
        if (auth_type) {
                char in[FTP_BUFSIZ], out[FTP_BUFSIZ];
                int length, kerror;

		strcpy(in, buf);
		if (debug) /* debugging output :) */
    			syslog(LOG_DEBUG, "DEBUG(prior to encrypt: %s)", in);

                /* reply (based on level) */
                if (strcmp(auth_type, "GSSAPI") == 0) {
                        gss_buffer_desc in_buf, out_buf;
                        OM_uint32 maj_stat, min_stat;
                        int conf_state;

                        in_buf.value = in;
                        in_buf.length = strlen(in) + 1;
                        maj_stat = gss_seal(&min_stat, gcontext,
                                            prot_level == PROT_P, /* confidential */
                                            GSS_C_QOP_DEFAULT,
                                            &in_buf, &conf_state,
                                            &out_buf);
                        if (maj_stat != GSS_S_COMPLETE) {
                                /* generally need to deal */
/* ??? how can this work?
                                secure_gss_error(maj_stat, min_stat,
                                               (prot_level==PROT_P)?
                                                 "gss_seal ENC didn't complete":
                                                 "gss_seal MIC didn't complete");
*/
                        } else if ((prot_level == PROT_P) && !conf_state) {

/* how can this work???
                                secure_error("GSSAPI didn't encrypt message");
*/
                        } else {
                                memcpy(out, out_buf.value, 
                                       length=out_buf.length);
                                gss_release_buffer(&min_stat, &out_buf);
                        }
                }
		buf[0] = '\0';
                if (kerror = radix_encode(out, in, &length, 0)) {
                        syslog(LOG_ERR, "Couldn't encode reply (%s)",
                                        radix_error(kerror));
			strcpy(buf, in);
                } else {
			snprintf(buf, sizeof(buf), "%s%c%s",
				prot_level == PROT_P ? "632" : "631",
				flags & USE_REPLY_LONG ? '-' : ' ',
				in);
		}
	}
#endif

  if (debug) /* debugging output :) */
    syslog(LOG_DEBUG, "<--- %s", buf);

  /* Yes, you want the debugging output before the client output; wrapping
   * stuff goes here, you see, and you want to log the cleartext and send
   * the wrapped text to the client.
   */

  printf("%s\r\n", buf); /* and send it to the client */
#ifdef TRANSFER_COUNT
  byte_count_total += strlen(buf);
  byte_count_out += strlen(buf);
#endif
  fflush(stdout);
}

void reply(int n, char *fmt, ...)
{
  va_list ap;
  
  if (autospout != NULL) { /* deal with the autospout stuff... */
    char *p, *ptr = autospout;

    while (*ptr) {
      if ((p = strchr(ptr, '\n')) != NULL) /* step through line by line */
	*p = '\0';

      /* send a line...(note that this overrides dolreplies!) */
      vreply(USE_REPLY_LONG | USE_REPLY_NOTFMT, n, ptr, ap);

      if (p)
	ptr = p + 1; /* set to the next line... (\0 is handled in the while) */
      else
	break; /* oh, we're done; drop out of the loop */
    }

    if (autospout_free) { /* free autospout if necessary */
      free(autospout);
      autospout_free = 0;
    }
    autospout = 0; /* clear the autospout */
  }

  va_start(ap, fmt);

  /* send the reply */
  vreply(0L, n, fmt, ap);

  va_end(ap);
}

void lreply(int n, char *fmt, ...)
{
  va_list ap;

  if (!dolreplies) /* prohibited from doing long replies? */
    return;

  va_start(ap, fmt);

  /* send the reply */
  vreply(USE_REPLY_LONG, n, fmt, ap);

  va_end(ap);
}

void ack(char *s)
{
    reply(250, "%s command successful.", s);
}

void nack(char *s)
{
    reply(502, "%s command not implemented.", s);
}

void yyerror(char *s)
{
    char *cp;
    if (s == NULL || yyerrorcalled != 0) return;
    if ((cp = strchr(cbuf, '\n')) != NULL)
        *cp = '\0';
    reply(500, "'%s': command not understood.", cbuf);
    yyerrorcalled = 1;
    return;
}

void delete(char *name)
{
    struct stat st;
    char realname [MAXPATHLEN];

    /*
     * delete permission?
     */

    wu_realpath (name, realname, chroot_path);

    if ( (del_check(name)) == 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to delete %s",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to delete %s",
                        pw->pw_name, remoteident, realname);
        return;
    }

    if (lstat(name, &st) < 0) {
        perror_reply(550, name);
        return;
    }
    if ((st.st_mode & S_IFMT) == S_IFDIR) {
        uid_t uid;
        gid_t gid;
        int d_mode;
        int valid;
        int enforce = 0;

        /*
         * check the directory, can we rmdir here?
         */
        if ( (dir_check(name, &uid, &gid, &d_mode, &valid)) <= 0 )
        {
            if (log_security)
                if (anonymous)
                    syslog (LOG_NOTICE, "anonymous(%s) of %s tried to delete directory %s",
                            guestpw, remoteident, realname);
                else
                    syslog (LOG_NOTICE, "%s of %s tried to delete directory %s",
                            pw->pw_name, remoteident, realname);
            return;
        }

        if (rmdir(name) < 0) {
            if (log_security)
                if (anonymous)
                    syslog (LOG_NOTICE, "anonymous(%s) of %s tried to delete directory %s (permissions)",
                            guestpw, remoteident, realname);
                else
                    syslog (LOG_NOTICE, "%s of %s tried to delete directory %s (permissions)",
                            pw->pw_name, remoteident, realname);
            perror_reply(550, name);
            return;
        }
        goto done;
    }
    if (unlink(name) < 0) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to delete %s (permissions)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to delete %s (permissions)",
                        pw->pw_name, remoteident, realname);
        perror_reply(550, name);
        return;
    }
  done:
    {
        char path[MAXPATHLEN];

        wu_realpath(name, path, chroot_path);

    if ((st.st_mode & S_IFMT) == S_IFDIR)
        if (anonymous) {
            syslog(LOG_NOTICE, "%s of %s deleted directory %s", guestpw, remoteident, path);
        } else {
            syslog(LOG_NOTICE, "%s of %s deleted directory %s", pw->pw_name,
                   remoteident, path);
        }
    else
        if (anonymous) {
            syslog(LOG_NOTICE, "%s of %s deleted %s", guestpw, remoteident, path);
        } else {
            syslog(LOG_NOTICE, "%s of %s deleted %s", pw->pw_name, remoteident, path);
        }
    }

    ack("DELE");
}

void cwd(char *path)
{
    struct aclmember *entry = NULL;
    char cdpath[MAXPATHLEN + 1];

    if (chdir(path) < 0) {
        /* alias checking */
        while (getaclentry("alias", &entry) && ARG0 && ARG1 != NULL) {
            if (!strcasecmp(ARG0, path)) {
                if (chdir(ARG1) < 0)
                    perror_reply(550, path);
                else {
                    show_message(250, C_WD);
                    show_readme(250, C_WD);
                    ack("CWD");
                }
                return;
            }
        }
    /* check for "cdpath" directories. */
    entry = (struct aclmember *) NULL;
        while (getaclentry("cdpath", &entry) && ARG0 != NULL) {
            snprintf(cdpath, sizeof cdpath, "%s/%s", ARG0, path);
            if (chdir(cdpath) >= 0) {
                show_message(250, C_WD);
                show_readme(250, C_WD);
                ack("CWD");
                return;
            }
        }
        perror_reply(550,path);
    } else {
        show_message(250, C_WD);
        show_readme(250, C_WD);
        ack("CWD");
    }
}

void makedir(char *name)
{
	uid_t uid;
	gid_t gid;
	int   d_mode;
	mode_t oldumask;
	int   valid;
	uid_t oldid;
	char  path[MAXPATHLEN + 1];  /* for realpath() later  - cky */
	char realname[MAXPATHLEN];

    wu_realpath (name, realname, chroot_path);
    /*
     * check the directory, can we mkdir here?
     */
    if ( (dir_check(name, &uid, &gid, &d_mode, &valid)) <= 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to create directory %s",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to create directory %s",
                        pw->pw_name, remoteident, realname);
        return;
    }

    /*
     * check the filename, is it legal?
     */
    if ( (fn_check(name)) <= 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to create directory %s (bad filename)",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to create directory %s (bad filename)",
                        pw->pw_name, remoteident, realname);
        return;
    }

    oldumask = umask(0000);
    if (valid <= 0) {
        d_mode = 0777;
        umask(oldumask);
    }

    if (mkdir(name, d_mode) < 0) {
      if (errno == EEXIST){
        if (log_security)
          if (anonymous)
            syslog(LOG_NOTICE, "anonymous(%s) of %s tried to create directory %s (exists)",
                   guestpw, remoteident, realname);
          else
            syslog(LOG_NOTICE, "%s of %s tried to create directory %s (exists)",
                   pw->pw_name, remoteident, realname);
                   
	fb_realpath(name, path);
        reply(521, "\"%s\" directory exists", path);
      } else {
        if (log_security)
          if (anonymous)
            syslog(LOG_NOTICE, "anonymous(%s) of %s tried to create directory %s (permissions)",
                   guestpw, remoteident, realname);
          else
            syslog(LOG_NOTICE, "%s of %s tried to create directory %s (permissions)",
                   pw->pw_name, remoteident, realname);
        perror_reply(550, name);
      }
      umask(oldumask);
      return;
    }
    umask(oldumask);
    if (valid > 0) {
        oldid = geteuid();
        delay_signaling();
        seteuid((uid_t) 0);
        if ((chown(name, uid, gid)) < 0) {
            seteuid(oldid);
            enable_signaling();
            perror_reply(550, "chown");
            return;
        }
        seteuid(oldid);
        enable_signaling();
    }
    wu_realpath(name, path, chroot_path);
    if(anonymous) {
        syslog(LOG_NOTICE, "%s of %s created directory %s", guestpw, remoteident, path);
    } else {
        syslog(LOG_NOTICE, "%s of %s created directory %s", pw->pw_name, remoteident, path);
    }
    fb_realpath(name, path);
    /* According to RFC 959:
     *   The 257 reply to the MKD command must always contain the
     *   absolute pathname of the created directory.
     * This is implemented here using similar code to the PWD command.
     * XXX - still need to do `quote-doubling'.
     */
    reply(257, "\"%s\" new directory created.", path);
}

void removedir(char *name)
{
    uid_t uid;
    gid_t gid;
    int d_mode;
    int valid;
    char realname [MAXPATHLEN];

    wu_realpath (name, realname, chroot_path);

    /*
     * delete permission?
     */

    if ( (del_check(name)) == 0 )
        return;
    /*
     * check the directory, can we rmdir here?
     */
    if ( (dir_check(name, &uid, &gid, &d_mode, &valid)) <= 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to remove directory %s",
                        guestpw, remoteident, realname);
            else
                syslog (LOG_NOTICE, "%s of %s tried to remove directory %s",
                        pw->pw_name, remoteident, realname);
        return;
    }

    if (rmdir(name) < 0) {
        if (errno == EBUSY)
            perror_reply(450, name);
        else
        {
            if (log_security)
                if (anonymous)
                    syslog (LOG_NOTICE, "anonymous(%s) of %s tried to remove directory %s (permissions)",
                            guestpw, remoteident, realname);
                else
                    syslog (LOG_NOTICE, "%s of %s tried to remove directory %s (permissions)",
                            pw->pw_name, remoteident, realname);
            perror_reply(550, name);
        }
    }
    else
        {
            char path[MAXPATHLEN];

            wu_realpath(name, path, chroot_path);

            if (anonymous) {
                syslog(LOG_NOTICE, "%s of %s deleted directory %s", guestpw, remoteident, path);
            } else {
                syslog(LOG_NOTICE, "%s of %s deleted directory %s", pw->pw_name, remoteident, path);
            }
        ack("RMD");
        }
}

void pwd()
{
    char path[MAXPATHLEN + 1];
#ifndef MAPPING_CHDIR
#ifdef HAVE_GETCWD
    extern char *getcwd();
#else
    extern char *getwd(char *);
#endif
#endif /* MAPPING_CHDIR */

#ifdef HAVE_GETCWD
    if (getcwd(path,MAXPATHLEN) == (char *) NULL)
#else
    if (getwd(path) == (char *) NULL)
#endif
    fb_realpath (".", path);
    reply(257, "\"%s\" is current directory.", path);
}

char * renamefrom(char *name)
{
    struct stat st;
    struct aclmember *entry = NULL;	/* Added: fixes a bug.  _H*/

    if (lstat(name, &st) < 0) {
        perror_reply(550, name);
        return ((char *) 0);
    }
    reply(350, "File exists, ready for destination name");
    return (name);
}

void renamecmd(char *from, char *to)
{
    int allowed = (anonymous ? 0 : 1);
    char realfrom[MAXPATHLEN];
    char realto[MAXPATHLEN];
#ifdef PARANOID
    struct stat st;
#endif
    wu_realpath (from, realfrom, chroot_path);
    wu_realpath (to, realto, chroot_path);
    /*
     * check the filename, is it legal?
     */
    if ( (fn_check(to)) == 0 )
    {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to rename %s to \"%s\" (bad filename)",
                        guestpw, remoteident, realfrom, realto);
            else
                syslog (LOG_NOTICE, "%s of %s tried to rename %s to \"%s\" (bad filename)",
                        pw->pw_name, remoteident, realfrom, realto);
        return;
    }

    /* 
     * if rename permission denied and file exists... then deny the user
     * permission to rename the file. 
     */
    while (getaclentry("rename", &entry) && ARG0 && ARG1 != NULL) {
        if (type_match(ARG1))
            if (anonymous) {
                if (*ARG0 == 'y')
                    allowed = 1;
            } else if (*ARG0 == 'n')
                allowed = 0;
    }
    if (! allowed) {
        if(log_security) {
            if (anonymous) {
                syslog(LOG_NOTICE, "anonymous(%s) of %s tried to rename %s to %s",
                       guestpw, remoteident, realfrom, realto);
            } else {
                syslog(LOG_NOTICE, "%s of %s tried to rename %s to %s",
                       pw->pw_name, remoteident, realfrom, realto);
            }
        }
        reply(553, "%s: Permission denied. (rename)", from);
        return;
    }


#ifdef PARANOID
/* Almost forgot about this.  Don't allow renaming TO existing files --
   otherwise someone can rename "trivial" to "warez", and "warez" is gone!
   XXX: This part really should do the same "overwrite" check as store(). */
    if (!stat(to, &st)) {
      if(log_security) {
        if (anonymous) {
            syslog(LOG_NOTICE, "anonymous(%s) of %s tried to rename %s to %s",
                   guestpw, remoteident, realfrom, realto);
          } else {
              syslog(LOG_NOTICE, "%s of %s tried to rename %s to %s",
                     pw->pw_name, remoteident, realfrom, realto);
          }
      }
      reply (550, "%s: Permission denied. (rename)", to);
      return;
    }
#endif
    if (rename(from, to) < 0) {
      if (log_security)
        if (anonymous)
          syslog(LOG_NOTICE, "anonymous(%s) of %s tried to rename %s to %s",
                 guestpw, remoteident, realfrom, realto);
        else
          syslog(LOG_NOTICE, "%s of %s tried to rename %s to %s",
                 pw->pw_name, remoteident, realfrom, realto);
      perror_reply(550, "rename");
    } else {
            char frompath[MAXPATHLEN];
            char topath[MAXPATHLEN];

            wu_realpath(from, frompath, chroot_path);
            wu_realpath(to, topath, chroot_path);

            if (anonymous) {
                syslog(LOG_NOTICE, "%s of %s renamed %s to %s", guestpw, remoteident, frompath, topath);
            } else {
                syslog(LOG_NOTICE, "%s of %s renamed %s to %s", pw->pw_name,
                       remoteident, frompath, topath);
            }
        ack("RNTO");
        }
}

void dolog(struct sockaddr_in *sin)
{
#ifndef NO_DNS
    struct hostent *hp;
	char *blah;

#ifdef	DNS_TRYAGAIN
    int num_dns_tries = 0;
    extern int h_errno;
    /*
     * 27-Apr-93    EHK/BM
     * far away connections might take some time to get their IP address
     * resolved. That's why we try again -- maybe our DNS cache has the
     * PTR-RR now. This code is sloppy. Far better is to check what the
     * resolver returned so that in case of error, there's no need to
     * try again.
     * This is done in BeroFTPD 1.1.12 -- 08-Sep-98 Bero
     */
dns_again:
#endif
     hp = gethostbyaddr((char *) &sin->sin_addr,
                                sizeof (struct in_addr), AF_INET);
#ifdef DNS_TRYAGAIN
     if ( !hp && h_errno==TRY_AGAIN && ++num_dns_tries <= 1 ) {
        sleep(3);
        goto dns_again;         /* try DNS lookup once more     */
     }
#endif

    blah = inet_ntoa(sin->sin_addr);

    strncpy(remoteaddr, blah, sizeof(remoteaddr));

    if (!strcasecmp(remoteaddr, "0.0.0.0")) {
        nameserved = 1;
        strncpy(remotehost, "localhost", sizeof(remotehost));
    } else {
        if (hp) {
            nameserved = 1;
            strncpy(remotehost, hp->h_name, sizeof(remotehost));
        } else {
            nameserved = 0;
            strncpy(remotehost, remoteaddr, sizeof(remotehost));
        }
    }
#else
    char *blah;

    blah = inet_ntoa(sin->sin_addr);
    (void) strncpy(remoteaddr, blah, sizeof(remoteaddr));
    nameserved = 0;
    (void) strncpy(remotehost, remoteaddr, sizeof(remotehost));
#endif

    remotehost[sizeof(remotehost)-1]='\0';
    sprintf(proctitle, "%s: connected", remotehost);
    setproctitle(proctitle);

#ifdef DAEMON
    if (be_daemon && logging)
        syslog(LOG_INFO, "connection from %s", remoteident);
#endif
}

/* Record logout in wtmp file and exit with supplied status. */

void dologout(int status)
{
     /*
      * Prevent reception of SIGURG from resulting in a resumption
      * back to the main program loop.
      */
     transflag = 0;
 
    if (logged_in) {
        delay_signaling(); /* we can't allow any signals while euid==0: kinch */
        seteuid((uid_t) 0);
	if (wtmp_logging)
            wu_logwtmp(ttyline, pw->pw_name, remotehost, 0);
    }
    if (logging)
	syslog(LOG_INFO, "FTP session closed");
    if (xferlog)
        close(xferlog);
    acl_remove();
    close (data);		/* H* fix: clean up a little better */
    close (pdata);
    /* beware of flushing buffers after a SIGPIPE */
    _exit(status);
}

void myoob(int sig)
{
    char *cp;

    /* only process if transfer occurring */
    if (!transflag) {
#ifdef SIGURG
        signal(SIGURG, myoob);
#endif
        return;
    }
    cp = tmpline;
    if (wu_getline(cp, sizeof(tmpline)-1, stdin) == NULL) {
        reply(221, "You could at least say goodbye.");
        dologout(0);
    }
    upper(cp);
    if (strcasecmp(cp, "ABOR\r\n") == 0) {
        tmpline[0] = '\0';
        reply(426, "Transfer aborted. Data connection closed.");
        reply(226, "Abort successful");
#ifdef SIGURG
        signal(SIGURG, myoob);
#endif
        if (ftwflag > 0) {
            ftwflag++;
            return;
        }
        longjmp(urgcatch, 1);
    }
    if (strcasecmp(cp, "STAT\r\n") == 0) {
        tmpline[0] = '\0';
        if (file_size != (off_t) - 1)
		reply(213, "Status: %" PRINTF_QD " of %" PRINTF_QD " bytes transferred",
                  byte_count, file_size);
        else
            reply(213, "Status: %" PRINTF_QD " bytes transferred", byte_count);
    }
#ifdef SIGURG
    signal(SIGURG, myoob);
#endif
}

/* Note: a response of 425 is not mentioned as a possible response to the
 * PASV command in RFC959. However, it has been blessed as a legitimate
 * response by Jon Postel in a telephone conversation with Rick Adams on 25
 * Jan 89. */

void passive()
{
    size_t len;
    int bind_error;
    register char *p,
     *a;

/* H* fix: if we already *have* a passive socket, close it first.  Prevents
   a whole variety of entertaining clogging attacks. */
    if (pdata > 0)
	close (pdata);
    if (!logged_in) {
       reply(530, "Login with USER first.");
       return;
    }
    pdata = socket(AF_INET, SOCK_STREAM, 0);
    if (pdata < 0) {
        perror_reply(425, "Can't open passive connection");
        return;
    }
    if (TCPwindowsize) {
        setsockopt(pdata, SOL_SOCKET, SO_SNDBUF, (char *) &TCPwindowsize, sizeof(TCPwindowsize));
        setsockopt(pdata, SOL_SOCKET, SO_RCVBUF, (char *) &TCPwindowsize, sizeof(TCPwindowsize));
    }
if (route_vectored)
    pasv_addr = vect_addr;
else
    pasv_addr = ctrl_addr;
    pasv_addr.sin_port = 0;
    delay_signaling(); /* we can't allow any signals while euid==0: kinch */
    seteuid((uid_t) 0);		/* XXX: not needed if > 1024 */
checkports();
if (passive_port_min ==  -1) {
    if (bind(pdata, (struct sockaddr *) &pasv_addr, sizeof(pasv_addr)) < 0) {
        seteuid((uid_t) pw->pw_uid);
        enable_signaling(); /* we can allow signals once again: kinch */
        goto pasv_error;
    }
} else {
    pasv_addr.sin_port = passive_port_max + 1;
    do
        if (pasv_addr.sin_port == 0 || --pasv_addr.sin_port < passive_port_min)
            bind_error = -1;
        else
            bind_error = bind ( pdata,(struct sockaddr *)&pasv_addr,sizeof(pasv_addr));
    while (bind_error<0 && errno==EADDRINUSE);
    if ( bind_error < 0 ) {
        seteuid((uid_t) pw->pw_uid);
        enable_signaling(); /* we can allow signals once again: kinch */
        goto pasv_error;
    }
}
    seteuid((uid_t) pw->pw_uid);
    enable_signaling(); /* we can allow signals once again: kinch */
    len = sizeof(pasv_addr);
    if (getsockname(pdata, (struct sockaddr *) &pasv_addr, &len) < 0)
        goto pasv_error;
    if (listen(pdata, 1) < 0)
        goto pasv_error;
    usedefault = 1;
    a = (char *) &pasv_addr.sin_addr;
    p = (char *) &pasv_addr.sin_port;

#define UC(b) (((int) b) & 0xff)

    reply(227, "Entering Passive Mode (%d,%d,%d,%d,%d,%d)", UC(a[0]),
          UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));
    return;

  pasv_error:
    close(pdata);
    pdata = -1;
    perror_reply(425, "Can't open passive connection");
    return;
}

/* Generate unique name for file with basename "local". The file named
 * "local" is already known to exist. Generates failure reply on error. */
char * gunique(char *local)
{
    static char new[MAXPATHLEN];
    struct stat st;
    char *cp = strrchr(local, '/');
    int count = 0;

    if (cp)
        *cp = '\0';
    if (stat(cp ? local : ".", &st) < 0) {
        perror_reply(553, cp ? local : ".");
        return ((char *) 0);
    }
    if (cp)
        *cp = '/';
    strncpy(new, local, (sizeof new) - 3);
    new[sizeof(new) - 3] = '\0';
    cp = new + strlen(new);
    *cp++ = '.';
    for (count = 1; count < 100; count++) {
        if (count == 10) {
            cp-=2;
            *cp++ = '.';
        }
        sprintf(cp, "%d", count);
        if (stat(new, &st) < 0)
            return (new);
    }
    reply(452, "Unique file name cannot be created.");
    return ((char *) 0);
}

/* Format and send reply containing system error number. */

void perror_reply(int code, char *string)
{
    reply(code, "%s: %s.", string, strerror(errno));
}

static char *onefile[] =
{"", 0};

void send_file_list(char *whichfiles)
{
    /* static so not clobbered by longjmp(), volatile would also work */
    static FILE *dout;
    static DIR *dirp;
    static char **sdirlist;

    struct stat st;

#ifdef HAVE_DIRENT_H
    struct dirent *dir;
#else
    struct direct *dir;
#endif

    register char **dirlist,
     *dirname;
    int simple = 0;
/*    char *strpbrk(const char *, const char *);
    Commented out because:
    - This is nonsense; should be done in <string.h> or <strings.h>
    - breaks glibc 2.1
    
    Let's see if someone complains...
    */

#if defined(TRANSFER_COUNT) && defined(TRANSFER_LIMIT)
    if ( ((file_limit_raw_out   > 0) && (xfer_count_out   >= file_limit_raw_out  ))
    ||   ((file_limit_raw_total > 0) && (xfer_count_total >= file_limit_raw_total))
    ||   ((data_limit_raw_out   > 0) && (byte_count_out   >= data_limit_raw_out  ))
    ||   ((data_limit_raw_total > 0) && (byte_count_total >= data_limit_raw_total)) ) {
        if (log_security)
            if (anonymous)
                syslog (LOG_NOTICE, "anonymous(%s) of %s tried to list files (Transfer limits exceeded)",
                        guestpw, remoteident);
            else
                syslog (LOG_NOTICE, "%s of %s tried to list files (Transfer limits exceeded)",
                        pw->pw_name, remoteident);
        reply(553, "Permission denied. (Transfer limits exceeded)");
        return;
    }
#endif

    dout = NULL;
    dirp = NULL;
    sdirlist = NULL;
    if (strpbrk(whichfiles, "~{[*?") != NULL) {
        extern char **ftpglob(register char *v),
         *globerr;

        globerr = NULL;
        dirlist = ftpglob(whichfiles);
        sdirlist = dirlist;  /* save to free later */
        if (globerr != NULL) {
            reply(550, globerr);
            goto globfree;
        } else if (dirlist == NULL) {
            errno = ENOENT;
            perror_reply(550, whichfiles);
            return;
        }
    } else {
        onefile[0] = whichfiles;
        dirlist = onefile;
        simple = 1;
    }

    if (setjmp(urgcatch)) {
        transflag = 0;
        if (dout != NULL)
            fclose(dout);
        if (dirp != NULL)
            closedir(dirp);
        data = -1;
        pdata = -1;
        goto globfree;
    }
    while ((dirname = *dirlist++) != NULL) {
        if (stat(dirname, &st) < 0) {
            /* If user typed "ls -l", etc, and the client used NLST, do what
             * the user meant. */
            if (dirname[0] == '-' && *dirlist == NULL && transflag == 0) {
                retrieve_is_data = 0;
#ifndef INTERNAL_LS
                retrieve(ls_plain, dirname);
#else
		ls(dirname,1);
#endif
                retrieve_is_data = 1;
                goto globfree;
            }
            perror_reply(550, dirname);
            if (dout != NULL) {
                fclose(dout);
                transflag = 0;
                data = -1;
                pdata = -1;
            }
            goto globfree;
        }
        if ((st.st_mode & S_IFMT) == S_IFREG) {
            if (dout == NULL) {
                dout = dataconn("file list", (off_t) - 1, "w");
                if (dout == NULL)
                    goto globfree;
                transflag++;
            }
            fprintf(dout, "%s%s\n", dirname,
                    type == TYPE_A ? "\r" : "");
            byte_count += strlen(dirname) + 1;
#ifdef TRANSFER_COUNT
            byte_count_total += strlen(dirname) + 1;
            byte_count_out += strlen(dirname) + 1;
            if (type == TYPE_A) {
                 byte_count_total++;
                 byte_count_out++;
            }
#endif
            continue;
        } else if ((st.st_mode & S_IFMT) != S_IFDIR)
            continue;

        if ((dirp = opendir(dirname)) == NULL)
            continue;

        while ((dir = readdir(dirp)) != NULL) {
            char nbuf[MAXPATHLEN];

#if !defined(HAVE_DIRENT_H) || defined(_DIRENT_HAVE_D_NAMLEN)    /* does not have d_namlen */
            if (dir->d_name[0] == '.' && dir->d_namlen == 1)
#else
            if (dir->d_name[0] == '.' && (strlen(dir->d_name) == 1))
#endif
                continue;
#if !defined(HAVE_DIRENT_H) || defined(_DIRENT_HAVE_D_NAMLEN)    /* does not have d_namlen */
            if (dir->d_namlen == 2 && dir->d_name[0] == '.' &&
                dir->d_name[1] == '.')
#else
            if ((strlen(dir->d_name) == 2) && dir->d_name[0] == '.' &&
                dir->d_name[1] == '.')
#endif
                continue;

            snprintf(nbuf, sizeof nbuf, "%s/%s", dirname, dir->d_name);

            /* We have to do a stat to insure it's not a directory or special
             * file. */
            if (simple || (stat(nbuf, &st) == 0 &&
                           (st.st_mode & S_IFMT) == S_IFREG)) {
                if (dout == NULL) {
                    dout = dataconn("file list", (off_t) -1,
                                    "w");
                    if (dout == NULL) {
                        closedir(dirp);
                        goto globfree;
                    }
                    transflag++;
                }
                if (nbuf[0] == '.' && nbuf[1] == '/')
                    fprintf(dout, "%s%s\n", &nbuf[2],
                            type == TYPE_A ? "\r" : "");
                else
                    fprintf(dout, "%s%s\n", nbuf,
                            type == TYPE_A ? "\r" : "");
                byte_count += strlen(nbuf) + 1;
#ifdef TRANSFER_COUNT
                byte_count_total += strlen(nbuf) + 1;
                byte_count_out += strlen(nbuf) + 1;
                if (type == TYPE_A) {
                    byte_count_total++;
                    byte_count_out++;
                }
#endif
            }
        }
        closedir(dirp);
        dirp = NULL;
    }

    if (dout == NULL)
        reply(550, "No files found.");
    else if (ferror(dout) != 0)
        perror_reply(550, "Data connection");
    else
#ifdef TRANSFER_COUNT
        {
        xfer_count_total++;
        xfer_count_out++;
#endif
        reply(226, "Transfer complete.");
#ifdef TRANSFER_COUNT
        }
#endif

    transflag = 0;
    if (dout != NULL)
        fclose(dout);
    data = -1;
    pdata = -1;
globfree:
    if (sdirlist) {
        blkfree(sdirlist);
        free((char *) sdirlist);
    }
}

/*
**  SETPROCTITLE -- set process title for ps
**
**	Parameters:
**		fmt -- a printf style format string.
**		a, b, c -- possible parameters to fmt.
**
**	Returns:
**		none.
**
**	Side Effects:
**		Clobbers argv of our main procedure so ps(1) will
**		display the title.
*/

#define SPT_NONE	0	/* don't use it at all */
#define SPT_REUSEARGV	1	/* cover argv with title information */
#define SPT_BUILTIN	2	/* use libc builtin */
#define SPT_PSTAT	3	/* use pstat(PSTAT_SETCMD, ...) */
#define SPT_PSSTRINGS	4	/* use PS_STRINGS->... */
#define SPT_SYSMIPS	5	/* use sysmips() supported by NEWS-OS 6 */
#define SPT_SCO		6	/* write kernel u. area */
#define SPT_CHANGEARGV	7	/* write our own strings into argv[] */

#ifndef SPT_TYPE
# define SPT_TYPE	SPT_REUSEARGV
#endif

#if SPT_TYPE != SPT_NONE && SPT_TYPE != SPT_BUILTIN

# if SPT_TYPE == SPT_PSTAT
#  include <sys/pstat.h>
# endif
# if SPT_TYPE == SPT_PSSTRINGS
#  include <machine/vmparam.h>
#  include <sys/exec.h>
#  ifndef PS_STRINGS	/* hmmmm....  apparently not available after all */
#   undef SPT_TYPE
#   define SPT_TYPE	SPT_REUSEARGV
#  else
#   ifndef NKPDE			/* FreeBSD 2.0 */
#    define NKPDE 63
typedef unsigned int	*pt_entry_t;
#   endif
#  endif
# endif

# if SPT_TYPE == SPT_PSSTRINGS || SPT_TYPE == SPT_CHANGEARGV
#  define SETPROC_STATIC	static
# else
#  define SETPROC_STATIC
# endif

# if SPT_TYPE == SPT_SYSMIPS
#  include <sys/sysmips.h>
#  include <sys/sysnews.h>
# endif

# if SPT_TYPE == SPT_SCO
#  include <sys/immu.h>
#  include <sys/dir.h>
#  include <sys/user.h>
#  include <sys/fs/s5param.h>
#  if PSARGSZ > MAXLINE
#   define SPT_BUFSIZE	PSARGSZ
#  endif
# endif

# ifndef SPT_PADCHAR
#  define SPT_PADCHAR	' '
# endif

# ifndef SPT_BUFSIZE
#  define SPT_BUFSIZE	MAXLINE
# endif

#endif /* SPT_TYPE != SPT_NONE && SPT_TYPE != SPT_BUILTIN */

#if SPT_TYPE != SPT_BUILTIN


/*VARARGS1*/
void setproctitle(const char *fmt, ...)
{
# if SPT_TYPE != SPT_NONE
	register char *p;
	register int i;
	SETPROC_STATIC char buf[SPT_BUFSIZE];
	va_list ap;
#  if SPT_TYPE == SPT_PSTAT
	union pstun pst;
#  endif
#  if SPT_TYPE == SPT_SCO
	off_t seek_off;
	static int kmem = -1;
	static int kmempid = -1;
	struct user u;
#  endif

	p = buf;

	/* print ftpd: heading for grep */
	strcpy(p, "ftpd: ");
	p += strlen(p);

	/* print the argument string */
	va_start(ap, fmt);
	vsnprintf(p, SPACELEFT(buf, p), fmt, ap);
	va_end(ap);

	/* strip off trailing newlines (they look ugly!) */
	p+=strlen(p);
	while(p>buf && *--p=='\n')
		*p=0;

	i = strlen(buf);

#  if SPT_TYPE == SPT_PSTAT
	pst.pst_command = buf;
	pstat(PSTAT_SETCMD, pst, i, 0, 0);
#  endif
#  if SPT_TYPE == SPT_PSSTRINGS
	PS_STRINGS->ps_nargvstr = 1;
	PS_STRINGS->ps_argvstr = buf;
#  endif
#  if SPT_TYPE == SPT_SYSMIPS
	sysmips(SONY_SYSNEWS, NEWS_SETPSARGS, buf);
#  endif
#  if SPT_TYPE == SPT_SCO
	if (kmem < 0 || kmempid != getpid())
	{
		if (kmem >= 0)
			close(kmem);
		kmem = open(_PATH_KMEM, O_RDWR, 0);
		if (kmem < 0)
			return;
		fcntl(kmem, F_SETFD, 1);
		kmempid = getpid();
	}
	buf[PSARGSZ - 1] = '\0';
	seek_off = UVUBLK + (off_t) u.u_psargs - (off_t) &u;
	if (lseek(kmem, (off_t) seek_off, SEEK_SET) == seek_off)
		write(kmem, buf, PSARGSZ);
#  endif
#  if SPT_TYPE == SPT_REUSEARGV
	if (i > LastArgv - Argv[0] - 2)
	{
		i = LastArgv - Argv[0] - 2;
		buf[i] = '\0';
	}
	strcpy(Argv[0], buf);
	p = &Argv[0][i];
	while (p < LastArgv)
		*p++ = SPT_PADCHAR;
	Argv[1] = NULL;
#  endif
#  if SPT_TYPE == SPT_CHANGEARGV
	Argv[0] = buf;
	Argv[1] = 0;
#  endif
# endif /* SPT_TYPE != SPT_NONE */
}

#endif /* SPT_TYPE != SPT_BUILTIN */

#ifdef KERBEROS
/* thanks to gshapiro@wpi.wpi.edu for the following kerberosities */

void init_krb()
{
    char hostname[100];

#ifdef HAVE_SYS_SYSTEMINFO_H
    if (sysinfo(SI_HOSTNAME, hostname, sizeof (hostname)) < 0) {
        perror("sysinfo");
#else
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
#endif
        exit(1);
    }
    if (strchr(hostname, '.'))
        *(strchr(hostname, '.')) = 0;

    sprintf(krb_ticket_name, "/var/dss/kerberos/tkt/tkt.%d", getpid());
    krb_set_tkt_string(krb_ticket_name);

    config_auth();

    if (krb_svc_init("hesiod", hostname, (char *) NULL, 0, (char *) NULL,
                     (char *) NULL) != KSUCCESS) {
        fprintf(stderr, "Couldn't initialize Kerberos\n");
        exit(1);
    }
}

void end_krb()
{
    unlink(krb_ticket_name);
}
#endif /* KERBEROS */

#ifdef ULTRIX_AUTH
static int
ultrix_check_pass(char *passwd, char *xpasswd)
{
    struct svcinfo *svp;
    int auth_status;

    if ((svp = getsvc()) == (struct svcinfo *) NULL) {
        syslog(LOG_WARNING, "getsvc() failed in ultrix_check_pass");
        return -1;
    }
    if (pw == (struct passwd *) NULL) {
        return -1;
    }
    if (((svp->svcauth.seclevel == SEC_UPGRADE) &&
        (!strcmp(pw->pw_passwd, "*")))
        || (svp->svcauth.seclevel == SEC_ENHANCED)) {
        if ((auth_status=authenticate_user(pw, passwd, "/dev/ttypXX")) >= 0) {
            /* Indicate successful validation */
            return auth_status;
        }
        if (auth_status < 0 && errno == EPERM) {
            /* Log some information about the failed login attempt. */
            switch(abs(auth_status)) {
            case A_EBADPASS:
                break;
            case A_ESOFTEXP:
                syslog(LOG_NOTICE, "password will expire soon for user %s",
                    pw->pw_name);
                break;
            case A_EHARDEXP:
                syslog(LOG_NOTICE, "password has expired for user %s",
                    pw->pw_name);
                break;
            case A_ENOLOGIN:
                syslog(LOG_NOTICE, "user %s attempted login to disabled acct",
                    pw->pw_name);
                break;
            }
        }
    }
    else {
        if ((*pw->pw_passwd != '\0') && (!strcmp(xpasswd, pw->pw_passwd))) {
            /* passwd in /etc/passwd isn't empty && encrypted passwd matches */
            return 0;
        }
    }
    return -1;
}
#endif /* ULTRIX_AUTH */

#ifdef DAEMON
/* I am running as a standalone daemon (not under inetd) */
void do_daemon(int argc, char **argv, char **envp)
{
  struct sockaddr_in server;
  struct servent *serv;
  int pgrp;
  int lsock;
  int one = 1;
  int status;
  int fd;
  FILE *pidfile;
  int i;

  /* Some of this is "borrowed" from inn - lots of it isn't */

if (be_daemon == 2) {
  /* Fork - so I'm not the owner of the process group any more */
  i = fork();
  if (i < 0) {
    syslog(LOG_ERR, "cant fork %m");
    exit(1);
  }
  /* No need for the parent any more */
  if (i > 0)
    exit(0);

/*
  pgrp = setpgrp( 0, getpid() );
*/
  pgrp = setsid();
  if( pgrp < 0 ){
    syslog( LOG_ERR, "cannot daemonise: %m" );
    exit( 1 );
  }
}

  if (! Bypass_PID_Files)
  if( (pidfile = fopen( _PATH_FTPD_PID, "w" )) ){
    fprintf( pidfile, "%d\n", getpid() );
    fclose( pidfile );
  }
  else {
    syslog( LOG_ERR, "Cannot write pidfile: %m" );
  }

  /* Close off all file descriptors and reopen syslog */
if (be_daemon == 2)
  {
    int i, fds;
#ifdef HAVE_GETRLIMIT
        struct rlimit rlp;
 
                rlp.rlim_cur = rlp.rlim_max = RLIM_INFINITY;
                if (getrlimit( RLIMIT_NOFILE, &rlp ) )
                        return;
                fds = rlp.rlim_cur;
#elif defined(HAVE_GETDTABLESIZE)
        if ((fds = getdtablesize()) <= 0)
            return;
#elif defined(OPEN_MAX)
    fds=OPEN_MAX; /* need to include limits.h somehow */
#else
    fds = sizeof(long); /* XXX -- magic */
#endif

    closelog ();
    for( i = 0; i <= fds; i++ ){
      close( i );
    }
#ifdef FACILITY
    openlog("ftpd", LOG_PID | LOG_NDELAY, FACILITY);
#else
    openlog("ftpd", LOG_PID);
#endif

    /* junk stderr */
    freopen(_PATH_DEVNULL, "w", stderr);
  }

  if ( RootDirectory != NULL ) {
    if ((chroot (RootDirectory) < 0)
    ||  (chdir ("/") < 0)) {
      syslog (LOG_ERR, "Cannot chroot to initial directory, aborting.");
      exit (1);
    }
    free (RootDirectory);
    RootDirectory = NULL;
  }

  if ( ! use_accessfile )
    syslog (LOG_WARNING, "FTP server started without ftpaccess file");

  syslog( LOG_INFO, "FTP server (%s) ready.", version);

  /* Create a socket to listen on */
  lsock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  if( lsock < 0 ){
    syslog( LOG_ERR, "Cannot create socket to listen on: %m" );
    exit( 1 );
  }
  if( setsockopt( lsock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof( one ) ) < 0 ){
    syslog( LOG_ERR, "Cannot set SO_REUSEADDR option: %m" );
    exit( 1 );
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  if(daemon_port == 0) {
      if( !(serv = getservbyname( "ftp", "tcp" )) ){
        syslog( LOG_ERR, "Cannot find service ftp: %m" );
        exit( 1 );
      }
      server.sin_port = serv->s_port;
      daemon_port=ntohs(serv->s_port);
  } else {
      server.sin_port = htons(daemon_port);
  }
      
  if( bind( lsock, (struct sockaddr *)&server, sizeof( server ) ) < 0 ){
    syslog( LOG_ERR, "Cannot bind socket: %m" );
    exit( 1 );
  }

  listen( lsock, MAX_BACKLOG );

  sprintf(proctitle, "accepting connections on port %i", daemon_port);
  setproctitle("%s", proctitle);

  while( 1 ){
    int pid;
    int msgsock;

    msgsock = accept( lsock, 0, 0 );
    if( msgsock < 0 ){
      syslog( LOG_ERR, "Accept failed: %m" );
      sleep( 1 );
      continue;
    }

    /* Fork off a handler */
    pid = fork();
    if( pid < 0 ){
      syslog( LOG_ERR, "failed to fork: %m" );
      sleep( 1 );
      continue;
    }
    if( pid == 0 ){
      /* I am that forked off child */
      closelog();
      /* Make sure that stdin/stdout are the new socket */
      dup2( msgsock, 0 );
      dup2( msgsock, 1 );
      /* Only parent needs lsock */
      if( lsock != 0 && lsock != 1 )
      close( lsock );
#ifdef FACILITY
    openlog("ftpd", LOG_PID | LOG_NDELAY, FACILITY);
#else
    openlog("ftpd", LOG_PID);
#endif
      return;
    }
    
    /* I am the parent */
    close( msgsock );

    /* Quick check to see if any of the forked off children have
     * terminated. */
    while( (pid = waitpid( (pid_t) -1, (int *)0, WNOHANG)) > 0 ){
      /* A child has finished */
    }
  }
}
#endif /* DAEMON */

#ifdef RATIO
int is_downloadfree(char *fname)
{
    char	rpath[MAXPATHLEN];
    char	*cp;

    if( !(anonymous || guest) )
	return 1;

    if( wu_realpath(fname,rpath,chroot_path) == NULL )
	return 0;

    while( getaclentry("dl-free-dir",&entry) ) {
	if( ARG0 == NULL || ARG1 == NULL )
	    continue;
	if( strcmp(ARG0,"*") != 0
	   && (!( anonymous && strcmp(ARG0,"anonymous") == 0  ))
	   && (!( guest && strcmp(ARG0,"guest") == 0 )) )
	    continue;
	if( strncmp(rpath,ARG1,strlen(ARG1)) == 0 ) {
	    return 1;
	}
    }
    while( getaclentry("dl-free",&entry) ) {
	if( ARG0 == NULL || ARG1 == NULL )
	    continue;
	if( strcmp(ARG0,"*") != 0
	   && (!( anonymous && strcmp(ARG0,"anonymous") == 0  ))
	   && (!( guest && strcmp(ARG0,"guest") == 0 )) )
	    continue;

	if( *(ARG1) != '/' ) {	/* compare basename */
	    if( (cp = strrchr(rpath,'/')) == NULL ) {
		cp = rpath;
	    }
	    else {
		++cp;
	    }
	    if( strcmp(cp,ARG1) == 0 ) {
		return 1;
	    }
	}
	else {	/* compare real path */
	    int		len;
	    cp = ARG1;
	    len = strlen(cp);
	    if( len > 2 && strcmp(cp+len-2, "/*")==0 ) { /* compare dirname */
		char	dname[MAXPATHLEN];
		strcpy(dname,ARG1);
		cp = dname;
		*(cp+len-2) = '\0';
		cp = strrchr(rpath,'/');
		if( cp != NULL ) {
		    *cp = '\0';
		}
		if( strcmp(dname,rpath) == 0 ) {
		    return 1;
		}
		else {
		    continue;
		}
	    }
	    else {
		if( strcmp(rpath,ARG1) == 0 ) {
		    return 1;
		}
	    }
	}
    }
    return 0;
}
#endif /* RATIO */



#ifdef KRB5
static krb5_context ftpd_context = 0;
/* Returns 0 on failure, 1 on success */
int check_krb5_password(const char *user, const char *pass)
{
       krb5_error_code    retval;
       krb5_principal     princ;
       krb5_creds         creds;
       krb5_ccache                ccache;
       char               cache_name[64];
       char *             princ_name;
       krb5_get_init_creds_opt opts;

       if (!ftpd_context) {
          if (retval = krb5_init_context(&ftpd_context))
              return 0;
          krb5_init_ets(ftpd_context);
       }
       krb5_get_init_creds_opt_init(&opts);

       if (retval = krb5_parse_name(ftpd_context, user, &princ))
           return 0;

       (void) sprintf(cache_name, "FILE:/tmp/ftpdcc_%ld", getpid());
       if (retval = krb5_cc_resolve(ftpd_context, cache_name, &ccache))
           return 0;

       if (retval = krb5_get_init_creds_password(ftpd_context, &creds, princ,
                                                 pass, krb5_prompter_ftpd,
                                                 NULL, 0, NULL, &opts))
           return 0;

       /* Stash the TGT so we can verify it. */
       if (retval = krb5_cc_initialize(ftpd_context, ccache, princ))
           return 0;
       if (retval = krb5_cc_store_cred(ftpd_context, ccache, &creds)) {
           (void) krb5_cc_destroy(ftpd_context, ccache);
           return 0;
       }

       retval = verify_krb_v5_tgt(ccache);
       (void) krb5_cc_destroy(ftpd_context, ccache);
       return (retval != -1);
}

/*
 * This routine with some modification is from the MIT V5B6 appl/bsd/login.c
 *
 * Verify the Kerberos ticket-granting ticket just retrieved for the
 * user.  If the Kerberos server doesn't respond, assume the user is
 * trying to fake us out (since we DID just get a TGT from what is
 * supposedly our KDC).  If the host/<host> service is unknown (i.e.,
 * the local keytab doesn't have it), let her in.
 *
 * Returns 1 for confirmation, -1 for failure, 0 for uncertainty.
 */
static int verify_krb_v5_tgt(krb5_ccache ccache)
{
    char               phost[BUFSIZ];
    krb5_error_code    retval;
    krb5_principal     princ;
    krb5_keyblock *    keyblock = 0;
    krb5_data          packet;
    krb5_auth_context  auth_context = NULL;
    packet.data = 0;

    /*
     * Get the server principal for the local host.
     * (Use defaults of "host" and canonicalized local name.)
     */
    if (krb5_sname_to_principal(ftpd_context, NULL, NULL,
                               KRB5_NT_SRV_HST, &princ))
       return -1;

    /* Extract the name directly. */
    strncpy(phost, krb5_princ_component(c, princ, 1)->data, BUFSIZ);
    phost[BUFSIZ - 1] = '\0';

    /*
     * Do we have host/<host> keys?
     * (use default keytab, kvno IGNORE_VNO to get the first match,
     * and enctype is currently ignored anyhow.)
     */
    if (retval = krb5_kt_read_service_key(ftpd_context, NULL, princ, 0,
                                         ENCTYPE_DES_CBC_MD5, &keyblock)) {
       /* Keytab or service key does not exist */
       retval = 0;
       goto cleanup;
    }
    if (keyblock)
       krb5_free_keyblock(ftpd_context, keyblock);

    /* Talk to the kdc and construct the ticket. */
    retval = krb5_mk_req(ftpd_context, &auth_context, 0, "host", phost,
                         NULL, ccache, &packet);
    if (auth_context) {
       krb5_auth_con_free(ftpd_context, auth_context);
       auth_context = NULL; /* setup for rd_req */
    }
    if (retval) {
       retval = -1;
       goto cleanup;
    }

    /* Try to use the ticket. */
    retval = krb5_rd_req(ftpd_context, &auth_context, &packet, princ,
                        NULL, NULL, NULL);
    if (retval) {
       retval = -1;
    } else {
       retval = 1;
    }

cleanup:
    if (packet.data)
       krb5_free_data_contents(ftpd_context, &packet);
    krb5_free_principal(ftpd_context, princ);
    return retval;
}
/*
 * Prompter function for krb5. If we do get called, just return a failure,
 * since the FTP protocol (as defined in RFC959) is rather inflexible here.
 * If we use RFC 2228, we can do this but presently there are no clients
 * that will understand this.
 */
krb5_error_code krb5_prompter_ftpd(krb5_context context, void *data,
                                  const char *banner, int num_prompts,
                                  krb5_prompt prompts[])
{
    return KRB5_LIBOS_CANTREADPWD;
}

reply_gss_error(code, maj_stat, min_stat, s)
int code;
OM_uint32 maj_stat, min_stat;
char *s;
{
        /* a lot of work just to report the error */
        OM_uint32 gmaj_stat, gmin_stat;
        gss_buffer_desc msg;
        int msg_ctx;
        msg_ctx = 0;
        while (!msg_ctx) {
                gmaj_stat = gss_display_status(&gmin_stat, maj_stat,
                                               GSS_C_GSS_CODE,
                                               GSS_C_NULL_OID,
                                               &msg_ctx, &msg);
                if ((gmaj_stat == GSS_S_COMPLETE)||
                    (gmaj_stat == GSS_S_CONTINUE_NEEDED)) {
                        lreply(code, "GSSAPI error major: %s", 
                               (char*)msg.value);
                        gss_release_buffer(&gmin_stat, &msg);
                }
                if (gmaj_stat != GSS_S_CONTINUE_NEEDED)
                        break;
        }
        msg_ctx = 0;
        while (!msg_ctx) {
                gmaj_stat = gss_display_status(&gmin_stat, min_stat,
                                               GSS_C_MECH_CODE,
                                               GSS_C_NULL_OID,
                                               &msg_ctx, &msg);
                if ((gmaj_stat == GSS_S_COMPLETE)||
                    (gmaj_stat == GSS_S_CONTINUE_NEEDED)) {
                        lreply(code, "GSSAPI error minor: %s",
                               (char*)msg.value);
                        gss_release_buffer(&gmin_stat, &msg);
                }
                if (gmaj_stat != GSS_S_CONTINUE_NEEDED)
                        break;
        }
        reply(code, "GSSAPI error: %s", s);
}

static char *radixN =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

radix_encode(unsigned char inbuf[], unsigned char outbuf[], int *len, int decode)
{
        int i,j,D=0;
        char *p;
        unsigned char c=0;

        if (decode) {
                for (i=0,j=0; inbuf[i] && inbuf[i] != '='; i++) {
                    if ((p = strchr(radixN, inbuf[i])) == NULL) return(1);
                    D = p - radixN;
                    switch (i&3) {
                        case 0:
                            outbuf[j] = D<<2;
                            break;
                        case 1:
                            outbuf[j++] |= D>>4;
                            outbuf[j] = (D&15)<<4;
                            break;
                        case 2:
                            outbuf[j++] |= D>>2;
                            outbuf[j] = (D&3)<<6;
                            break;
                        case 3:
                            outbuf[j++] |= D;
                    }
                }
                switch (i&3) {
                        case 1: return(3);
                        case 2: if (D&15) return(3);
                                if (strcmp((char *)&inbuf[i], "==")) return(2);
                                break;
                        case 3: if (D&3) return(3);
                                if (strcmp((char *)&inbuf[i], "="))  return(2);
                }
                *len = j;
        } else {
                for (i=0,j=0; i < *len; i++)
                    switch (i%3) {
                        case 0:
                            outbuf[j++] = radixN[inbuf[i]>>2];
                            c = (inbuf[i]&3)<<4;
                            break;
                        case 1:
                            outbuf[j++] = radixN[c|inbuf[i]>>4];
                            c = (inbuf[i]&15)<<2;
                            break;
                        case 2:
                            outbuf[j++] = radixN[c|inbuf[i]>>6];
                            outbuf[j++] = radixN[inbuf[i]&63];
                            c = 0;
                    }
                if (i%3) outbuf[j++] = radixN[c];
                switch (i%3) {
                        case 1: outbuf[j++] = '=';
                        case 2: outbuf[j++] = '=';
                }
                outbuf[*len = j] = '\0';
        }
        return(0);
}

char *
radix_error(e)
{
        switch (e) {
            case 0:  return("Success");
            case 1:  return("Bad character in encoding");
            case 2:  return("Encoding not properly padded");
            case 3:  return("Decoded # of bits not a multiple of 8");
            default: return("Unknown error");
        }
}


setlevel(new_level)
int new_level;
{
        switch (new_level) {
                case PROT_S:
                case PROT_P:
                        if (auth_type)
                case PROT_C:
                                reply(200, "Protection level set to %s.",
                                        (prot_level = new_level) == PROT_S ?
                                                "Safe" : prot_level == PROT_P ?
                                                "Private" : "Clear");
                        else
                default:        reply(536, "%s protection level not supported.",
                                        prot_levelnames[new_level]);
        }
}

auth(type)
char *type;
{
        if (auth_type)
                reply(534, "Authentication type already set to %s", auth_type);
        else
        if (strcmp(type, "GSSAPI") == 0)
                reply(334, "Using authentication type %s; ADAT must follow",
                                temp_auth_type = type);
        else
        /* Other auth types go here ... */
                reply(504, "Unknown authentication type: %s", type);
}


auth_data(data)
char *data;
{
        int kerror, length;

        if (auth_type) {
                reply(503, "Authentication already established");
                return(0);
        }
        if (!temp_auth_type) {
                reply(503, "Must identify AUTH type before ADAT");
                return(0);
        }
        if (strcmp(temp_auth_type, "GSSAPI") == 0) {
                int replied = 0;
                int found = 0;
                gss_cred_id_t server_creds;     
                gss_name_t client;
                int ret_flags;
                struct gss_channel_bindings_struct chan;
                gss_buffer_desc name_buf;
                gss_name_t server_name;
                OM_uint32 acquire_maj, acquire_min, accept_maj, accept_min,
                                stat_maj, stat_min;
                gss_OID mechid;
                gss_buffer_desc tok, out_tok;
                char gbuf[FTP_BUFSIZ];
                u_char gout_buf[FTP_BUFSIZ];
                char localname[MAXHOSTNAMELEN];
                char service_name[MAXHOSTNAMELEN+10];
                char **service;
                struct hostent *hp;

                chan.initiator_addrtype = GSS_C_AF_INET;
                chan.initiator_address.length = 4;
                chan.initiator_address.value = &his_addr.sin_addr.s_addr;
                chan.acceptor_addrtype = GSS_C_AF_INET;
                chan.acceptor_address.length = 4;
                chan.acceptor_address.value = &ctrl_addr.sin_addr.s_addr;
                chan.application_data.length = 0;
                chan.application_data.value = 0;

                if (kerror = radix_encode(data, gout_buf, &length, 1)) {
                        reply(501, "Couldn't decode ADAT (%s)",
                              radix_error(kerror));
                        syslog(LOG_ERR, "Couldn't decode ADAT (%s)",
                               radix_error(kerror));
                        return(0);
                }
                tok.value = gout_buf;
                tok.length = length;

                if (gethostname(localname, MAXHOSTNAMELEN)) {
                        reply(501, "couldn't get local hostname (%d)\n", errno);
                        syslog(LOG_ERR, "Couldn't get local hostname (%d)", errno);
                        return 0;
                }
                if (!(hp = gethostbyname(localname))) {
                        extern int h_errno;
                        reply(501, "couldn't canonicalize local hostname (%d)\n", h_errno);
                        syslog(LOG_ERR, "Couldn't canonicalize local hostname (%d)", h_errno);
                        return 0;
                }
                strcpy(localname, hp->h_name);

                for (service = gss_services; *service; service++) {
                        sprintf(service_name, "%s@%s", *service, localname);
                        name_buf.value = service_name;
                        name_buf.length = strlen(name_buf.value) + 1;
                        if (debug)
                                syslog(LOG_INFO, "importing <%s>", service_name);
                        stat_maj = gss_import_name(&stat_min, &name_buf, 
                                                   gss_nt_service_name,
                                                   &server_name);
                        if (stat_maj != GSS_S_COMPLETE) {
                                reply_gss_error(501, stat_maj, stat_min,
                                                "importing name");
                                syslog(LOG_ERR, "gssapi error importing name");
                                return 0;
                        }

                        acquire_maj = gss_acquire_cred(&acquire_min, server_name, 0,
                                                       GSS_C_NULL_OID_SET, GSS_C_ACCEPT,
                                                       &server_creds, NULL, NULL);
                        gss_release_name(&stat_min, &server_name);

                        if (acquire_maj != GSS_S_COMPLETE)
			{
/*
				reply_gss_error(501, acquire_maj, acquire_min, "acquire_cred");
*/
                                continue;
			}

                        found++;

                        gcontext = GSS_C_NO_CONTEXT;

                        accept_maj = gss_accept_sec_context(&accept_min,
                                                            &gcontext, /* context_handle */
                                                            server_creds, /* verifier_cred_handle */
                                                            &tok, /* input_token */
                                                            &chan, /* channel bindings */
                                                            &client, /* src_name */
                                                            &mechid, /* mech_type */
                                                            &out_tok, /* output_token */
                                                            &ret_flags,
                                                            NULL,       /* ignore time_rec */
                                                            NULL   /* ignore del_cred_handle */
                                                            );
                        if (accept_maj==GSS_S_COMPLETE||accept_maj==GSS_S_CONTINUE_NEEDED)
                                break;
                }

                if (found) {
                        if (accept_maj!=GSS_S_COMPLETE && accept_maj!=GSS_S_CONTINUE_NEEDED) {
                                reply_gss_error(535, accept_maj, accept_min,
                                                "accepting context");
                                syslog(LOG_ERR, "failed accepting context");
                                gss_release_cred(&stat_min, &server_creds);
                                return 0;
                        }
                } else {
                        reply_gss_error(501, stat_maj, stat_min,
                                        "acquiring credentials");
                        syslog(LOG_ERR, "gssapi error acquiring credentials");
                        return 0;
                }

                if (out_tok.length) {
                        if (kerror = radix_encode(out_tok.value, gbuf, &out_tok.length, 0)) {
                                reply(535,"Couldn't encode ADAT reply (%s)",
                                             radix_error(kerror));
                                syslog(LOG_ERR, "couldn't encode ADAT reply");
                                return(0);
                        }
                        if (stat_maj == GSS_S_COMPLETE) {
                                reply(235, "ADAT=%s", gbuf);
                                replied = 1;
                        } else {
                                /* If the server accepts the security data, and
                                   requires additional data, it should respond with
                                   reply code 335. */
                                reply(335, "ADAT=%s", gbuf);
                        }
                        gss_release_buffer(&stat_min, &out_tok);
                }
                if (stat_maj == GSS_S_COMPLETE) {
                        /* GSSAPI authentication succeeded */
                        stat_maj = gss_display_name(&stat_min, client, &client_name, 
                                                    &mechid);
                        if (stat_maj != GSS_S_COMPLETE) {
                                /* "If the server rejects the security data (if 
                                   a checksum fails, for instance), it should 
                                   respond with reply code 535." */
                                reply_gss_error(535, stat_maj, stat_min,
                                                "extracting GSSAPI identity name");
                                syslog(LOG_ERR, "gssapi error extracting identity");
                                gss_release_cred(&stat_min, &server_creds);
                                return 0;
                        }
                        /* If the server accepts the security data, but does
                                   not require any additional data (i.e., the security
                                   data exchange has completed successfully), it must
                                   respond with reply code 235. */
                        if (!replied) reply(235, "GSSAPI Authentication succeeded");

                        auth_type = temp_auth_type;
                        temp_auth_type = NULL;

                        gss_release_cred(&stat_min, &server_creds);
                        return(1);
                } else if (stat_maj == GSS_S_CONTINUE_NEEDED) {
                        /* If the server accepts the security data, and
                                   requires additional data, it should respond with
                                   reply code 335. */
                        reply(335, "more data needed");
                        gss_release_cred(&stat_min, &server_creds);
                        return(0);
                } else {
                        /* "If the server rejects the security data (if 
                                   a checksum fails, for instance), it should 
                                   respond with reply code 535." */
                        reply_gss_error(535, stat_maj, stat_min, 
                                        "GSSAPI failed processing ADAT");
                        syslog(LOG_ERR, "GSSAPI failed processing ADAT");
                        gss_release_cred(&stat_min, &server_creds);
                        return(0);
                }
        }
	return(0);
}

#ifdef KRB5
secure_gss_error(OM_uint maj_stat, OM_uint min_stat, char *s)
{
    return reply_gss_error(535, maj_stat, min_stat, s);
}

secure_error(char *fmt, ...)
{
    char buf[FTP_BUFSIZ];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    reply(535, "%s", buf);
    syslog(LOG_ERR, "%s", buf);
}
#endif

/* ftpd_userok -- hide details of getting the name and verifying it */
/* returns 0 for OK */
ftpd_userok(client_name, name)
        gss_buffer_t client_name;
        char *name;
{
        int retval = -1;
        krb5_boolean k5ret;
        krb5_context kc;
        krb5_principal p;
        krb5_error_code kerr;

        kerr = krb5_init_context(&kc);
        if (kerr)
                return -1;

        kerr = krb5_parse_name(kc, client_name->value, &p);
        if (kerr) { retval = -1; goto fail; }
        k5ret = krb5_kuserok(kc, p, name);
        if (k5ret == TRUE)
                retval = 0;
        else 
                retval = 1;
        krb5_free_principal(kc, p);
 fail:
        krb5_free_context(kc);
        return retval;
}

#endif

#ifdef AFS
int check_afs_password(char *user, char *passwd)
{
      char reason[500];
      char afsuser[500], afspass[500];
      int password_expires;
      struct passwd savepw;
      int code;

/* save current pw struct */
      memcpy(&savepw, pw, sizeof(savepw));

      strcpy(afsuser, user);
      strcpy(afspass, passwd);

      setpag();
      code = ka_UserAuthenticateGeneral(
              KA_USERAUTH_VERSION,  afsuser, "",
              "", afspass, 0, &password_expires, 0, reason);

/* restore old pw struct */
      memcpy(pw, &savepw, sizeof(savepw));

      return (code == 0);
}
#endif

#ifdef USE_PAM
/* This is rather an abuse of PAM, but the FTP protocol doesn't allow much
 * flexibility here.  :-(
 */

#include <security/pam_appl.h>
/* Static variables used to communicate between the conversation function
 * and the server_login function
 */
static char *PAM_password;

/* PAM conversation function
 * Here we assume (for now, at least) that echo on means login name, and
 * echo off means password.
 */
static int
PAM_conv (int num_msg, const struct pam_message **msg,
struct pam_response **resp, void *appdata_ptr)
{
    int replies = 0;
    struct pam_response *reply = NULL;

#   define COPY_STRING(s) (s) ? strdup(s) : NULL

    reply = malloc(sizeof(struct pam_response) * num_msg);
    if (!reply) return PAM_CONV_ERR;

    for (replies = 0; replies < num_msg; replies++) {
        switch (msg[replies]->msg_style) {
            case PAM_PROMPT_ECHO_ON:
                return PAM_CONV_ERR;
                break;
            case PAM_PROMPT_ECHO_OFF:
                reply[replies].resp_retcode = PAM_SUCCESS;
	        reply[replies].resp = COPY_STRING(PAM_password);
                  /* PAM frees resp */
                break;
            case PAM_TEXT_INFO:
                /* ignore it... */
                reply[replies].resp_retcode = PAM_SUCCESS;
	        reply[replies].resp = NULL;
                break;
              case PAM_ERROR_MSG:
                /* ignore it... */
                reply[replies].resp_retcode = PAM_SUCCESS;
	        reply[replies].resp = NULL;
                break;
            default:
                /* Must be an error of some sort... */
                return PAM_CONV_ERR;
        }
    }
    *resp = reply;
    return PAM_SUCCESS;
}
static struct pam_conv PAM_conversation = {
    &PAM_conv,
    NULL
};

static int
pam_check_pass(char *user, char *passwd)
{
    pam_handle_t *pamh;
    int pam_error;

    /* Now use PAM to do authentication.  For now, we won't worry about
     * session logging, only authentication.  Bail out if there are any
     * errors.  Since this is a limited protocol, and an even more limited
     * function within a server speaking this protocol, we can't be as
     * verbose as would otherwise make sense.
     */
    #define PAM_BAIL if (pam_error != PAM_SUCCESS) { \
       pam_end(pamh, 0); return 0; \
     }
    PAM_password = passwd;
    pam_error = pam_start("ftp", user, &PAM_conversation, &pamh);
    pam_set_item(pamh, PAM_RHOST, remotehost);
    PAM_BAIL;
    pam_error = pam_authenticate(pamh, 0);
    PAM_BAIL;
    pam_error = pam_acct_mgmt(pamh, 0);
    PAM_BAIL;
    pam_error = pam_setcred(pamh, PAM_ESTABLISH_CRED);
    PAM_BAIL;
    pam_end(pamh, PAM_SUCCESS);
    /* If this point is reached, the user has been authenticated. */
    return 1;
}
#endif
