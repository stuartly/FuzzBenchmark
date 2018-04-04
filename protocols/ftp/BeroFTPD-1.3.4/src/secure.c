/* Originally from MIT Kerberos 5 distribution
 * Replaced obsolete K&R constructs with ANSI
 */
/*
 * Shared routines for client and server for
 * secure read(), write(), getc(), and putc().
 * Only one security context, thus only work on one fd at a time!
 */

#include "config.h"

#ifdef KRB5
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_generic.h>
extern gss_ctx_id_t gcontext;
#endif /* KRB5 */

#include "ftp.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>

extern int     prot_level;
extern char    *auth_type;

#define MAX maxbuf
extern unsigned int maxbuf;    /* maximum output buffer size */
extern unsigned char *ucbuf;   /* cleartext buffer */
static unsigned int nout, bufp;        /* number of chars in ucbuf,
                                * pointer into ucbuf */

#ifdef KRB5
#undef FUDGE_FACTOR
#define FUDGE_FACTOR 64 /*It appears to add 52 byts, but I'm not usre it is a constant--hartmans*/
#endif /*KRB5*/

#ifndef FUDGE_FACTOR           /* In case no auth types define it. */
#define FUDGE_FACTOR 0
#endif

/* perhaps use these in general, certainly use them for GSSAPI */

#ifndef looping_write
static int
looping_write(int fd, register const char *buf, int len)
{
    int cc;
    register int wrlen = len;

    do {
       cc = write(fd, buf, wrlen);
       if (cc < 0) {
           if (errno == EINTR)
               continue;
           return(cc);
       }
       else {
           buf += cc;
           wrlen -= cc;
       }
    } while (wrlen > 0);
    return(len);
}
#endif
#ifndef looping_read
static int
looping_read(int fd, register char *buf, register int len)
{
    int cc, len2 = 0;

    do {
       cc = read(fd, buf, len);
       if (cc < 0) {
           if (errno == EINTR)
               continue;
           return(cc);          /* errno is already set */
       }
       else if (cc == 0) {
           return(len2);
       } else {
           buf += cc;
           len2 += cc;
           len -= cc;
       }
    } while (len > 0);
    return(len2);
}
#endif


extern secure_error(char *, ...);

#define ERR    -2

static
secure_putbyte(int fd, unsigned char c)
{
       int ret;

       ucbuf[nout++] = c;
       if (nout == MAX - FUDGE_FACTOR) {
         nout = 0;
         ret = secure_putbuf(fd, ucbuf, MAX - FUDGE_FACTOR);
         return(ret?ret:c);
       }
return (c);
}

/* returns:
 *      0  on success
 *     -1  on error (errno set)
 *     -2  on security error
 */
secure_flush(int fd)
{
       int ret;

       if (prot_level == PROT_C)
               return(0);
       if (nout)
               if (ret = secure_putbuf(fd, ucbuf, nout))
                       return(ret);
       return(secure_putbuf(fd, "", nout = 0));
}

/* returns:
 *     c>=0  on success
 *     -1    on error
 *     -2    on security error
 */
secure_putc(char c, FILE *stream)
{
       if (prot_level == PROT_C)
               return(putc(c,stream));
       return(secure_putbyte(fileno(stream), (unsigned char) c));
}

/* returns:
 *     nbyte on success
 *     -1  on error (errno set)
 *     -2  on security error
 */
secure_write(int fd, unsigned char *buf, unsigned int nbyte)
{
       unsigned int i;
       int c;

       if (prot_level == PROT_C)
               return(write(fd,buf,nbyte));

       for (i=0; nbyte>0; nbyte--)
               if ((c = secure_putbyte(fd, buf[i++])) < 0)
                       return(c);
       return(i);
}

/* returns:
 *      0  on success
 *     -1  on error (errno set)
 *     -2  on security error
 */
secure_putbuf(int fd, unsigned char *buf, unsigned int nbyte)
{
       static char *outbuf = NULL;             /* output ciphertext */
       static unsigned int bufsize = 0;        /* size of outbuf */
       size_t length;
       size_t net_len;

       /* Other auth types go here ... */
#ifdef KRB5
       if (strcmp(auth_type, "GSSAPI") == 0) {
               gss_buffer_desc in_buf, out_buf;
               OM_uint32 maj_stat, min_stat;
               int conf_state;

               in_buf.value = buf;
               in_buf.length = nbyte;
               maj_stat = gss_seal(&min_stat, gcontext,
                                   (prot_level == PROT_P), /* confidential */
                                   GSS_C_QOP_DEFAULT,
                                   &in_buf, &conf_state,
                                   &out_buf);
               if (maj_stat != GSS_S_COMPLETE) {
                       /* generally need to deal */
                       /* ie. should loop, but for now just fail */
                       secure_gss_error(maj_stat, min_stat,
                                        prot_level == PROT_P?
                                        "GSSAPI seal failed":
                                        "GSSAPI sign failed");
                       return(ERR);
               }

               if (bufsize < out_buf.length) {
                       if (outbuf?
                           (outbuf = realloc(outbuf, (unsigned) out_buf.length)):
                           (outbuf = malloc((unsigned) out_buf.length))) {
                               bufsize = out_buf.length;
                       } else {
                               free(outbuf);
                               outbuf = NULL;
                               bufsize = 0;
                               secure_error("%s (in malloc of PROT buffer)",
                                            strerror(errno));
                               return(ERR);
                       }
               }

               memcpy(outbuf, out_buf.value, length=out_buf.length);
               gss_release_buffer(&min_stat, &out_buf);
       }
#endif /* KRB5 */
       net_len = htonl((u_long) length);
       if (looping_write(fd, &net_len, 4) == -1) return(-1);
       if (looping_write(fd, outbuf, length) != length) return(-1);
       return(0);
}


secure_puts(char *s, FILE *out)
{
    return (secure_write(fileno(out), s, strlen(s)));
}


static
secure_getbyte(int fd)
{
       /* number of chars in ucbuf, pointer into ucbuf */
       static unsigned int nin, bufp;
       int kerror;
       int32_t length;

       if (nin == 0) {
               if ((kerror = looping_read(fd, &length, sizeof(length)))
                               != sizeof(length)) {
                       secure_error("Couldn't read PROT buffer length: %d/%s",
                                    kerror,
                                    kerror == -1 ? strerror(errno)
                                    : "premature EOF");
                       return(ERR);
               }
               if ((length = (u_long) ntohl(length)) > MAX) {
                       secure_error("Length (%d) of PROT buffer > PBSZ=%u",
                                    length, MAX);
                       return(ERR);
               }
               if ((kerror = looping_read(fd, ucbuf, length)) != length) {
                       secure_error("Couldn't read %u byte PROT buffer: %s",
                                       length, kerror == -1 ?
                                       strerror(errno) : "premature EOF");
                       return(ERR);
               }
               /* Other auth types go here ... */
#ifdef KRB5
               if (strcmp(auth_type, "GSSAPI") == 0) {
                 gss_buffer_desc xmit_buf, msg_buf;
                 OM_uint32 maj_stat, min_stat;
                 int conf_state;

                 xmit_buf.value = ucbuf;
                 xmit_buf.length = length;
                 conf_state = (prot_level == PROT_P);
                 /* decrypt/verify the message */
                 maj_stat = gss_unseal(&min_stat, gcontext, &xmit_buf,
                                       &msg_buf, &conf_state, NULL);
                 if (maj_stat != GSS_S_COMPLETE) {
                   secure_gss_error(maj_stat, min_stat,
                                    (prot_level == PROT_P)?
                                    "failed unsealing ENC message":
                                    "failed unsealing MIC message");
                   return ERR;
                 }

                 memcpy(ucbuf, msg_buf.value, nin = bufp = msg_buf.length);
                 gss_release_buffer(&min_stat, &msg_buf);
             }
#endif /* KRB5 */
               /* Other auth types go here ... */
       }
       if (nin == 0)
               return(EOF);
       else    return(ucbuf[bufp - nin--]);
}

/* returns:
 *     c>=0 on success
 *     -1   on EOF
 *     -2   on security error
 */
secure_getc(FILE *stream)
{
       if (prot_level == PROT_C)
               return(getc(stream));
       return(secure_getbyte(fileno(stream)));
}

/* returns:
 *     n>0 on success (n == # of bytes read)
 *     0   on EOF
 *     -1  on error (errno set), only for PROT_C
 *     -2  on security error
 */
secure_read(int fd, char *buf, int nbyte)
{
       static int c;
       int i;

       if (prot_level == PROT_C)
               return(read(fd,buf,nbyte));
       if (c == EOF)
               return(c = 0);
       for (i=0; nbyte>0; nbyte--)
               switch (c = secure_getbyte(fd)) {
                       case ERR: return(c);
                       case EOF: if (!i) c = 0;
                                 return(i);
                       default:  buf[i++] = c;
               }
       return(i);
}
