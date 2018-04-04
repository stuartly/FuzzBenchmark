/* Some useful functions...                                       */
/* Version 1.1.0, last modified 97/08/18			  */
/* (c) 1998 by Bernhard Rosenkraenzer <bero@linux.net.eu.org>     */

#ifndef lint
static char sccsid[] = "@(#)$Id: tool.c,v 1.1.1.1 1998/08/21 18:10:34 root Exp $";
#endif /* not lint */

#include "tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VALIDCHAR "\t\n\r\b\000"

/* Functions... */

void abandon(int err, char *message)
{
	puts(message);
	exit(err);
}
                
char *extract(char *from, const char *begin, const char end)
{
	char *found,*result;

	found=strcasestr(from,begin);
	while(found) {
		if(strchr(VALIDCHAR,(int) (*(found-1))) || !(*(found-1))) {
			result=salloc(slen(from) + 1);
			strcpy(result,(char *) found+slen(begin));
			if((char *) strchr(result,end)!=NULL)
				result[strchr(result,end)-result]='\0';
			if((char *) strchr(result,'\n')!=NULL)
				result[strchr(result,'\n')-result]='\0';
			return(result);
		}
		found=strcasestr(found+1,begin);
	}
	return(NULL);
}

char *noleadingspaces(char *text)
{
	char *a=text;
	while(slen(a)>0 && a[0]==' ') a++;
	return(a);
}

char *readfile(char *filename)
{
	FILE *in;
	char *content;
	register int size, flen;

	size=1024; flen=0;
	in=fopen(filename,"r");
	if(in!=NULL) {
		content=salloc(sizeof(char)*(size+2));
		while(!feof(in)) {
			content[flen]=(char) fgetc(in);
			if(flen==size) {
				content[flen+1]='\0';
				size+=2048;
				content=(char *) realloc(content, sizeof(char)*(size+2));
			}
			flen++;
		}
		content[flen-1]='\0';
		fclose(in);
		return(content);
	} else return(NULL);
}

char *readline(FILE *file)
{
	char *line;
	register char last;
	register int len;
	int flen;
  
	line=salloc(sizeof(char)*1024);
	last=0; len=0; flen=1024*sizeof(char);
	while(!feof(file) && last!=EOF && last!='\n') {
		last=(char) fgetc(file);
		if(last!='\n' && !feof(file)) {
			line[len++]=last;
		}
		if(len==flen) {
			flen+=1024*sizeof(char);
			line=(char *) realloc(line,flen);
		}
	}
	return(line);
}

char *email(char *full_address)
{
	/* Get the plain address part from an e-mail address
	   (i.e. remove realname) */
  
	char *addr;
  
	addr=salloc(slen(full_address)+1);
	strcpy(addr, full_address);
  
	/* Realname <user@host> type address */
	if(((char *) strchr(addr,'<'))!=NULL) {
		addr=(char *) strchr(addr,'<')+1;
		addr[strchr(addr,'>')-addr]='\0';
	}
  
	/* user@host (Realname) type address */
	if(((char *) strchr(addr,' '))!=NULL)
		addr[strchr(addr,' ')-addr]='\0';

	return(addr);
}

char *realname(char *full_address)
{
	char *name;
	char *adr;
	adr=noleadingspaces(full_address);

	name=salloc(slen(adr)+1);
	strcpy(name,adr);

	/* Realname <user@host> type address */
	if(((char *) strchr(name,'<'))!=NULL) {
		/* Some mailers (pine) can create <user@host> addresses without a realname... :( */
		if(name[0]!='<')
			name[strchr(name,'<')-1-name]='\0';
		else {
			strcpy(name,name+1);
			name[strchr(name,'@')-name]='\0';
		}			
  	/* user@host (Realname) type address) */
	} else if(((char *) strchr(name,'('))!=NULL) {
		name=strchr(name,'(')+1;
		name[strchr(name,')')-name]='\0';
  	/* Realname not specified -> use username */
	} else
		if(strchr(name,'@')!=NULL)
			name[strchr(name,'@')-name]='\0';
			
	return(name);
}

char *username(char *address)
{
	char *name;
	name=email(address);
	if (strchr(name,'@') != NULL)
		name[strchr(name,'@')-name]='\0';
	return(name);
}

char *downcase(const char *s)
{
	register unsigned int i;
	unsigned int l;
	char *n;

	l=slen(s);
	n=salloc(l+1);
	for(i=0;i<l;i++)
		n[i]=tolower(s[i]);
/* For systems that don't have tolower, this should be a proper
   replacement... I'll have to get that autoconfed some time... ;)
		if(s[i]>='A' && s[i]<='Z')
			n[i]=s[i]-'A'+'a';
		else
			n[i]=s[i]; */
	return(n);
}

char *strcasestr(const char *s1, const char *s2)
{
	char *s3,*s4;
	s3=downcase(s1);
	s4=strstr(s3,downcase(s2));
	free(s3);
	if(s4!=NULL) {
		return (char*)s1+(s4-s3);
	} else
		return NULL;
}

char *strcasechr(const char *s, int c)
{
	char *s1,*s2;
	s1=downcase(s);
	s2=strchr(s1,tolower(c));
	free(s1);
	if(s2!=NULL)
		return (char*)s+(s2-s1);
	else
		return NULL;
}

char *salloc(size_t size)
{
	char *a;
	a=(char *) malloc(size);
	if(a!=NULL) memset(a,0,size); 
	return a;
}

#ifdef WEIRD_STRCASECMP
	int scasecmp(const char *s1, const char *s2)
	{
		if(s1==NULL && s2==NULL)
			return 0;
		else if(s1==NULL || s2==NULL)
			return -1;
		else
			return strcasecmp(s1,s2);
	}
	
	int scmp(const char *s1, const char *s2)
	{
		if(s1==NULL && s2==NULL)
			return 0;
		else if(s1==NULL || s2==NULL)
			return -1;
		else
			return strcmp(s1,s2);
	}
#endif

#ifdef WEIRD_STRLEN
	size_t slen(const char *s)
	{
		if(s==NULL)
			return 0;
		else
			return strlen(s);
	}
#endif

char exist(char *filename)
{
	FILE *temp;
	temp=fopen(filename,"r");
	if(temp==NULL)
		return 0;
	else {
		fclose(temp);
		return 1;
	}
}

char choice(char *prompt, char *choices)
{
	register char i;
	register char answer=0;
	printf("%s (",prompt);
	for(i=0; i<slen(choices); i++) {
		printf("%c",choices[i]);
		if(i<slen(choices)-1)
			printf("/");
	}
	printf(") ");
	while(answer==0) {
		answer=fgetc(stdin);
		for(i=0;i<slen(choices); i++)
			if(tolower(choices[i])==tolower(answer))
				return i;
		answer=0;
	}
	return 0; /* OK, this can't happen, but it stops compiler warnings... */
}

char yn(char *prompt)
{
	if(choice(prompt,"yn")==0)
		return 1;
	else
		return 0;
}

char *input(char *prompt, size_t maxsize)
{
	char *answer=salloc(maxsize);
	printf("%s ",prompt);
	/* Some (buggy?) implementations of fgets() seem to return a \n without waiting for input occasionally;
	   work around this: */
	while(fgets(answer,maxsize-1,stdin)==NULL || strchr(answer,'\n')==answer) {
	}
	if(strchr(answer,'\n')!=NULL)
		answer[strchr(answer,'\n')-answer]=0;
	return answer;
}
