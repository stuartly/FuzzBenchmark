/* When of the supported authentication methods the ftp server will attempt
 * to use.  Define as 1 to enable, 0 to disable. */

#ifdef USE_RFC931
#define USE_A_RFC931    1               /* Use RFC931-style authentication */
#else
#define USE_A_RFC931	0
#endif

/* Bitmasks used to identify authentication methods that returned a result */
#define A_RFC931        1 << 0;         /* RFC931 */
