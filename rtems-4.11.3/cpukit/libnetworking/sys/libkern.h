/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)libkern.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: src/sys/sys/libkern.h,v 1.48 2005/02/10 20:39:39 glebius Exp $
 */

#ifndef _SYS_LIBKERN_H_
#define	_SYS_LIBKERN_H_

#include <sys/cdefs.h>
#include <sys/types.h>

/* BCD conversions. */
extern u_char const bcd2bin_data[];
extern u_char const bin2bcd_data[];
extern char const hex2ascii_data[];

#define	bcd2bin(bcd)	(bcd2bin_data[bcd])
#define	bin2bcd(bin)	(bin2bcd_data[bin])
#define	hex2ascii(hex)	(hex2ascii_data[hex])

static __inline int imax (int a, int b)
{
	return (a > b ? a : b);
}

static __inline int imin (int a, int b)
{
	return (a < b ? a : b);
}

static __inline long lmax (long a, long b)
{
	return (a > b ? a : b);
}

static __inline long lmin (long a, long b)
{
	return (a < b ? a : b);
}

static __inline u_int max (u_int a, u_int b)
{
	return (a > b ? a : b);
}

static __inline u_int min (u_int a, u_int b)
{
	return (a < b ? a : b);
}

static __inline quad_t qmax (quad_t a, quad_t b)
{
	return (a > b ? a : b);
}

static __inline quad_t qmin (quad_t a, quad_t b)
{
	return (a < b ? a : b);
}

static __inline u_long ulmax (u_long a, u_long b)
{
	return (a > b ? a : b);
}

static __inline u_long ulmin (u_long a, u_long b)
{
	return (a < b ? a : b);
}

/* Prototypes for non-quad routines. */
int bcmp (const void *, const void *, size_t);
#ifndef HAVE_INLINE_FFS
int ffs (int);
#endif
#ifndef	HAVE_INLINE_FLS
int fls (int);
#endif
int locc (int, char *, u_int);
void qsort (void *base, size_t nmemb, size_t size,
			int (*compar) (const void *, const void *));
#if defined(__rtems__)
u_long rtems_bsdnet_random (void);
#define random()	rtems_bsdnet_random()
#else
u_long random (void);
#endif
char *index (const char *, int);
char *rindex (const char *, int);
int scanc (u_int, const u_char *, const u_char *, int);
int skpc (int, int, char *);
void srandom (u_long);
char *strcat (char *__restrict, const char *__restrict);
int strcmp (const char *, const char *);
char *strdup (const char *s);
char *strcpy (char *__restrict, const char *__restrict);
size_t strlen (const char *);
int strncmp (const char *, const char *, size_t);
char *strncpy (char *__restrict, const char *__restrict, size_t);
char *strerror (int errnum);

#endif /* !_SYS_LIBKERN_H_ */
