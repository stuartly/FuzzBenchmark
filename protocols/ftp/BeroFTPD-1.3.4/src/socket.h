/* Socket prototype declarations */

FILE *SockOpen(char *host, int clientPort);
char *SockGets(FILE *sockfp, char *buf, int len);
int SockWrite(char *buf, int size, int nels, FILE *sockfp);
int SockPrintf(FILE *sockfp, char *format, ...);
int SockPuts(FILE *sockfp, char *buf);
int Reply(FILE *sockfp);
int Send(FILE *sockfp, char *format, ...);
