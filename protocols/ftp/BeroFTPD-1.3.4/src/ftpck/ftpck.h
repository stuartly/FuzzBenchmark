/*
**  Subsystem:   WU-FTPD Configuration Checker
**  File Name:   ftpck.h
**                                                        
** This software is Copyright (c) 1997 by Kent Landfield
**
**
** Specify the path to your inetd.conf file.
*/

#define INETD_CONF   "/etc/inetd.conf"

/*
** Check the modes below and customize for your local 
** security/administrative policy.  Please be aware that
** what you see below is the recomended modes for the
** various WU-FTPD configuation and log files.
*/

#define FTPSERVERS_MODES       0600 
#define FTPPID_MODES           0644 
#define FTPACCESS_MODES        0600 
#define FTPCONVERSIONS_MODES   0600 
#define FTPGROUPS_MODES        0600 
#define FTPHOSTS_MODES         0600 
#define FTPUSERS_MODES         0600 
#define XFERLOG_MODES          0640 

/*
** Checking 'alias' and 'cdpath' directives need a little explaining here. 
** The problem is one of perspective. If you put aliases in your ftpaccess 
** file then the problem arises as to who gets to use them.  Aliases need to 
** be relative to the root structure the user logs in as.  If you login as a 
** real user then the path is a real path relative to your '/' directory.
** If I login as an anonymous user the path is based from the chrooted 
** environment.  
** 
** Many sites have alias and cdpath directives setup that are only usable 
** by one of the types of users (real or anonymous/virtual).  While maybe not 
** quite commpletely correct, there is no problem having alias and cdpath 
** entries that are not usable by all users.
**
** Do you wish to verify alias and cdpaths usable in all ftp directories ?
** This means that they must be pathed so they are available to real as well
** as anonymous/virtual users.  
**
** The CHECK_ALIASES_* and CHECK_CDPATH_* defines below control how the
** alias and cdpath directives are checked. Set the appropriate defines
** to '1' if you wish it checked.  '0' turns off the check.
**
** If you turn off both the ROOTDIR and ANONYMOUS for either the alias or
** cdpath directive then the only checks made for that directive is a 
** syntax/field check.
*/

#define CHECK_ROOTDIR_ALIASES      0
#define CHECK_ANONYMOUS_ALIASES    1

#define CHECK_ROOTDIR_CDPATH       0
#define CHECK_ANONYMOUS_CDPATH     1

