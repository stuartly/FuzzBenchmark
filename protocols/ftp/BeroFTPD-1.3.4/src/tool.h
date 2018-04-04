/* Some useful functions...                                       */
/* (c) 1997 by Bernhard Rosenkraenzer <bero@bero-online.ml.org>   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void abandon(int err, char *message);
char *extract(char *from, const char *begin, const char end);
char *readfile(char *filename);
char *readline(FILE *file);
char *email(char *full_address);
char *realname(char *full_address);
char *username(char *address);
char *downcase(const char *s);
char *strcasestr(const char *s1, const char *s2);
char *strcasechr(const char *s, int c);
char *salloc(size_t size);
char *noleadingspaces(char *text);

#ifdef WEIRD_STRCASECMP
  int scasecmp(const char *s1, const char *s2);
  int scmp(const char *s1, const char *s2);
#else
  #define scasecmp(x,y) strcasecmp(x,y)
  #define scmp(x,y) strcmp(x,y)
#endif
#ifdef WEIRD_STRLEN
  size_t slen(const char *s);
#else
  #define slen(x) strlen(x)
#endif  

char exist(char *filename);
char choice(char *prompt, char *choices);
char yn(char *prompt);
char *input(char *prompt, size_t maxsize);
