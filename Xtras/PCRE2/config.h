﻿/* config.h for CMake builds */

/* #undef HAVE_DIRENT_H */
#define HAVE_INTTYPES_H 1    
#define HAVE_STDINT_H 1                                                   
#define HAVE_STRERROR 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#ifdef _WIN32
#define HAVE_WINDOWS_H 1
#else
#define HAVE_UNISTD_H 1
#endif

/* #undef HAVE_BCOPY */
#define HAVE_MEMMOVE 1

/* #undef PCRE2_STATIC */

#define SUPPORT_PCRE2_8 1
/* #undef SUPPORT_PCRE2_16 */
/* #undef SUPPORT_PCRE2_32 */
/* #undef PCRE2_DEBUG */

/* #undef SUPPORT_LIBBZ2 */
/* #undef SUPPORT_LIBEDIT */
/* #undef SUPPORT_LIBREADLINE */
/* #undef SUPPORT_LIBZ */

/* #undef SUPPORT_JIT */
/* #undef SUPPORT_PCRE2GREP_JIT */
#define SUPPORT_UNICODE 1
/* #undef SUPPORT_VALGRIND */

/* #undef BSR_ANYCRLF */
/* #undef EBCDIC */
/* #undef EBCDIC_NL25 */
/* #undef HEAP_MATCH_RECURSE */
/* #undef NEVER_BACKSLASH_C */

#define LINK_SIZE		2
#define MATCH_LIMIT		10000000
#define MATCH_LIMIT_RECURSION	MATCH_LIMIT
#define NEWLINE_DEFAULT         4
#define PARENS_NEST_LIMIT       250
#define PCRE2GREP_BUFSIZE       20480

#define MAX_NAME_SIZE	32
#define MAX_NAME_COUNT	10000

/* end config.h for CMake builds */
