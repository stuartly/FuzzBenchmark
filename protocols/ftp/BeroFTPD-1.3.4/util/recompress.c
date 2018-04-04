/* Pretty generic file recompressor - it only insists on the decompressor
 * accepting the "-c" flag to decompress to stdout, which works for
 * gunzip, uncompress, bunzip2, tunzip, and more.
 * $Id: recompress.c,v 1.1.1.1 1998/08/21 18:10:35 root Exp $
 */
#include <stdio.h>

/* Some odd systems like SunOS don't have basename :/ */
#define lbasename(x) (strrchr(x,'/')?1+strrchr(x,'/'):x)

main (ac, av)
    int ac;
    char **av;
{
    char *zipfile;
    int fd[2];

    switch (ac) {

    case 4:
        zipfile = av[3];
        break;

    case 3:
        zipfile = NULL;
        break;

    default:
        fputs ("usage: recompress decompressor compressor [file]", stderr);
        fputs ("       Example: recompress /bin/uncompress /bin/gzip [file]", stderr);
        exit (1);
    }

    if (pipe (fd) < 0) {
        perror ("pipe");
        exit (1);
    }

    switch (fork ()) {

    default:            /* the father */
        if (dup2 (fd[0], 0) < 0) {
            perror ("parent: dup2");
            exit (1);
        }
        close (fd[1]);
        execlp (av[2], lbasename(av[2]), NULL);
        perror ("execlp: compressor");
        exit (1);

    case 0:             /* the son */
        if (dup2 (fd[1], 1) < 0) {
            perror ("child: dup2");
            exit (1);
        }
        close (fd[0]);
        execlp (av[1], lbasename(av[1]), "-c", zipfile, NULL);
        perror ("execlp: uncompressor");
        exit (1);

    case -1:            /* Murphy's ghost */
        perror ("fork");
        exit (1);
    }
}
