#!/bin/sh -
#
# Copyright (c) 1983 The Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)$Id: newvers.sh,v 1.1.1.1 1998/08/21 18:10:33 root Exp $ based on
#       newvers.sh	5.3 (Berkeley) 10/31/88
#
if [ ! -r edit ]; then echo 0 > edit; fi
touch edit
awk '	{	edit = $1 + 1; }\
END	{	printf "char version[] = \"BeroFTPD 1.3.4(%d) ", edit > "vers.c";\
#LANG=
#LC_TIME=
		printf "%d\n", edit > "edit"; }' < edit
echo `LC_TIME=C date`'";' >> vers.c
