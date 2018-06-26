﻿//dztall
#ifndef HEADER_CURL_CONFIG_ANDROID_H
#define HEADER_CURL_CONFIG_ANDROID_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* =================================================================== */
/*                Hand crafted config file for Android                 */
/* =================================================================== */

#define OS "android"

#define HAVE_SYS_POLL_H 1

/* Define if you want the built-in manual */
#define USE_MANUAL              1

#define HAVE_ERRNO_H            1
#define HAVE_NETINET_IN_H       1
#define HAVE_SYS_SOCKET_H       1
#define HAVE_SYS_SELECT_H       1
#define HAVE_NETDB_H            1
#define HAVE_ARPA_INET_H        1
#define HAVE_UNISTD_H           1
#define HAVE_NET_IF_H           1
#define HAVE_SYS_TYPES_H        1
#define HAVE_GETTIMEOFDAY       1
#define HAVE_FCNTL_H            1
#define HAVE_SYS_STAT_H         1
#define HAVE_ALLOCA_H           1
#define HAVE_STDLIB_H           1
#define HAVE_TIME_H             1
#define HAVE_UTIME_H            1
#define HAVE_SYS_TIME_H         1
#define HAVE_SYS_UTIME_H        1

//dztall
#define HAVE_LONGLONG			1

//dztall
#define CURL_EXTERN_SYMBOL extern __attribute__ ((visibility ("default")))

#define TIME_WITH_SYS_TIME      1

#define HAVE_ALARM              1
#define HAVE_FTRUNCATE          1
#define HAVE_UTIME              1
#define HAVE_SETVBUF            1
#define HAVE_STRFTIME           1
#define HAVE_INET_ADDR          1
#define HAVE_MEMCPY             1
#define HAVE_SELECT             1
#define HAVE_SOCKET             1
#define HAVE_STRUCT_TIMEVAL     1

#define HAVE_SIGACTION          1
#define HAVE_SIGNAL_H           1
#define HAVE_SIG_ATOMIC_T       1

//dztall.
#define USE_SSLEAY            1
#define USE_OPENSSL           1
#define CURL_WANTS_CA_BUNDLE_ENV	1

//dztall
#define USE_LIBSSH2 1
#define HAVE_LIBSSH2_H 1
#define HAVE_LIBSSH2_INIT 1
#define HAVE_LIBSSH2_EXIT 1

#define HAVE_STDBOOL_H 1
#define HAVE_BOOL_T 1

#define CURL_DISABLE_LDAP       1

#define HAVE_RAND_STATUS        1
#define HAVE_RAND_EGD           1

#define HAVE_IOCTL              1
#define HAVE_FCNTL_O_NONBLOCK 1
//#define HAVE_IOCTL_FIONBIO      1

#define RETSIGTYPE void

#define SIZEOF_LONG 4
#define SIZEOF_INT              4
#define SIZEOF_SHORT            2
#ifdef __LP64__
#define SIZEOF_SIZE_T           8
#define SIZEOF_CURL_OFF_T 8
#else
#define SIZEOF_SIZE_T           4
#define SIZEOF_CURL_OFF_T 4
#endif

#define HAVE_GETNAMEINFO 1
#define GETNAMEINFO_QUAL_ARG1 const
#define GETNAMEINFO_TYPE_ARG1 struct sockaddr *
#define GETNAMEINFO_TYPE_ARG2 socklen_t
#define GETNAMEINFO_TYPE_ARG46 size_t
#define GETNAMEINFO_TYPE_ARG7 int

#define HAVE_RECV 1
#define RECV_TYPE_ARG1 int
#define RECV_TYPE_ARG2 void *
#define RECV_TYPE_ARG3 size_t
#define RECV_TYPE_ARG4 int
#define RECV_TYPE_RETV ssize_t

#define HAVE_RECVFROM 1
#define RECVFROM_TYPE_ARG1 int
#define RECVFROM_TYPE_ARG2 void
#define RECVFROM_TYPE_ARG3 size_t
#define RECVFROM_TYPE_ARG4 int
#define RECVFROM_TYPE_ARG5 struct sockaddr
#define RECVFROM_TYPE_ARG6 int
#define RECVFROM_TYPE_RETV ssize_t
#define RECVFROM_TYPE_ARG2_IS_VOID 1

#define HAVE_SEND 1
#define SEND_TYPE_ARG1 int
#define SEND_QUAL_ARG2 const
#define SEND_TYPE_ARG2 void *
#define SEND_TYPE_ARG3 size_t
#define SEND_TYPE_ARG4 int
#define SEND_TYPE_RETV ssize_t

//#define HAVE_EXTRA_STRICMP_H 1
//#define HAVE_EXTRA_STRDUP_H  1

#endif /* HEADER_CURL_CONFIG_ANDROID_H */
