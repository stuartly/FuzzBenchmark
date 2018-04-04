#ifndef FNM_PATHNAME
#define FNM_PATHNAME    0x01    /* Slash must be matched by slash. */
#endif
#ifndef FNM_NOESCAPE
#define FNM_NOESCAPE    0x02    /* Disable backslash escaping. */
#endif
#ifndef FNM_PERIOD
#define FNM_PERIOD      0x04    /* Period must be matched by period. */
#endif

#define LOG_IN  0
#define C_WD    1
#define BANNER  2

#ifndef ALIGN
#define ALIGN(x)        ((x) + (sizeof(long) - (x) % sizeof(long)))
#endif

#define O_COMPRESS              (1 << 0)    /* file was compressed */
#define O_UNCOMPRESS            (1 << 1)    /* file was uncompressed */
#define O_TAR                   (1 << 2)    /* file was tar'ed */

#define MAXARGS         50
#define MAXKWLEN        20

struct aclmember {
    struct aclmember *next;
    char keyword[MAXKWLEN];
    char *arg[MAXARGS];
};

#define MAXUSERS        1024

#define ARG0    entry->arg[0]
#define ARG1    entry->arg[1]
#define ARG2    entry->arg[2]
#define ARG3    entry->arg[3]
#define ARG4    entry->arg[4]
#define ARG5    entry->arg[5]
#define ARG6    entry->arg[6]
#define ARG7    entry->arg[7]
#define ARG8    entry->arg[8]
#define ARG9    entry->arg[9]
#define ARG     entry->arg

#ifdef QUOTA
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

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_QUOTA_H
#define QUOTA_BLOCKS
#define QUOTA_DEVICE
#include <sys/quota.h>
#endif

#ifdef HAVE_UFS_QUOTA_H
#define QUOTA_BLOCKS
#define QUOTA_DEVICE
#include <ufs/quota.h>
#endif

#ifdef HAVE_JFS_QUOTA_H
#include <jfs/quota.h>
#endif

#ifdef HAVE_UFS_QUOTA_H
#include <ufs/quota.h>
#endif

#ifdef HAVE_UFS_UFS_QUOTA_H
#include <ufs/ufs/quota.h>
#endif

#if defined(HAVE_LINUX_QUOTA_H) && !defined(HAVE_SYS_QUOTA_H) /* Linux libc <= 6 */
#define QUOTA_DEVICE
#include <linux/quota.h>
#endif

#ifdef HAVE_PATHS_H
#include <paths.h>
#else
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif
#endif

#ifndef _PATH_MNTTAB
#define _PATH_MNTTAB MNTTAB
#endif
#endif /* QUOTA */
