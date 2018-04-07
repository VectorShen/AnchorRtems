/*-
 * Copyright (c) 1999 John D. Polstra
 * Copyright (c) 1999,2001 Peter Wemm <peter@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _SYS_LINKER_SET_H_
#define _SYS_LINKER_SET_H_

#ifndef _SYS_CDEFS_H_
#error this file needs sys/cdefs.h as a prerequisite
#endif

/*
 * The following macros are used to declare global sets of objects, which
 * are collected by the linker into a `linker_set' as defined below.
 * For ELF, this is done by constructing a separate segment for each set.
 */

/*
 * Private macros, not to be used outside this header file.
 */
#ifdef __GNUCLIKE___SECTION
#ifndef __rtems__
#define __MAKE_SET(set, sym)						\
	__GLOBL(__CONCAT(__start_set_,set));				\
	__GLOBL(__CONCAT(__stop_set_,set));				\
	static void const * const __set_##set##_sym_##sym 		\
	__section("set_" #set) __used = &sym
#else /* __rtems__ */
#define RTEMS_BSD_DEFINE_SET(set, type)				\
	type const __CONCAT(_bsd__start_set_,set)[0]			\
	__section(".rtemsroset.bsd." __STRING(set) ".begin") __used;		\
	type const __CONCAT(_bsd__stop_set_,set)[0]			\
	__section(".rtemsroset.bsd." __STRING(set) ".end") __used

#define RTEMS_BSD_DECLARE_SET(set, type)				\
	extern type const __CONCAT(_bsd__start_set_,set)[0];		\
	extern type const __CONCAT(_bsd__stop_set_,set)[0]

#define RTEMS_BSD_DEFINE_SET_ITEM(set, sym, type)			\
	static type const __set_##set##_sym_##sym 			\
	__section(".rtemsroset.bsd." __STRING(set) ".content") __used

#define __MAKE_SET(set, sym)						\
	RTEMS_BSD_DEFINE_SET_ITEM(set, sym, const void *) = &sym

#define RTEMS_BSD_DEFINE_RWSET(set, type)				\
	type __CONCAT(_bsd__start_set_,set)[0]			\
	__section(".rtemsrwset.bsd." __STRING(set) ".begin") __used;		\
	type __CONCAT(_bsd__stop_set_,set)[0]				\
	__section(".rtemsrwset.bsd." __STRING(set) ".end") __used

#define RTEMS_BSD_DECLARE_RWSET(set, type)				\
	extern type __CONCAT(_bsd__start_set_,set)[0];			\
	extern type __CONCAT(_bsd__stop_set_,set)[0]

#define RTEMS_BSD_DEFINE_RWSET_ITEM(set, sym, type)			\
	static type __set_##set##_sym_##sym 				\
	__section(".rtemsrwset.bsd." __STRING(set) ".content") __used

#define __MAKE_RWSET(set, sym)						\
	RTEMS_BSD_DEFINE_RWSET_ITEM(set, sym, const void *) = &sym
#endif /* __rtems__ */
#else /* !__GNUCLIKE___SECTION */
#ifndef lint
#error this file needs to be ported to your compiler
#endif /* lint */
#define __MAKE_SET(set, sym)	extern void const * const (__set_##set##_sym_##sym)
#endif /* __GNUCLIKE___SECTION */

/*
 * Public macros.
 */
#define TEXT_SET(set, sym)	__MAKE_SET(set, sym)
#define DATA_SET(set, sym)	__MAKE_SET(set, sym)
#define BSS_SET(set, sym)	__MAKE_SET(set, sym)
#define ABS_SET(set, sym)	__MAKE_SET(set, sym)
#define SET_ENTRY(set, sym)	__MAKE_SET(set, sym)
#ifdef __rtems__
#define RWDATA_SET(set, sym)	__MAKE_RWSET(set, sym)
#endif /* __rtems__ */

/*
 * Initialize before referring to a given linker set.
 */
#ifndef __rtems__
#define SET_DECLARE(set, ptype)						\
	extern ptype *__CONCAT(__start_set_,set);			\
	extern ptype *__CONCAT(__stop_set_,set)

#define SET_BEGIN(set)							\
	(&__CONCAT(__start_set_,set))
#define SET_LIMIT(set)							\
	(&__CONCAT(__stop_set_,set))
#else /* __rtems__ */
#define SET_DECLARE(set, ptype)						\
	RTEMS_BSD_DECLARE_SET(set, ptype *)

#define RWSET_DECLARE(set, ptype)					\
	RTEMS_BSD_DECLARE_RWSET(set, ptype *)

#define SET_BEGIN(set)							\
	(__CONCAT(_bsd__start_set_,set))
#define SET_LIMIT(set)							\
	(__CONCAT(_bsd__stop_set_,set))
#endif /* __rtems__ */

/*
 * Iterate over all the elements of a set.
 *
 * Sets always contain addresses of things, and "pvar" points to words
 * containing those addresses.  Thus is must be declared as "type **pvar",
 * and the address of each set item is obtained inside the loop by "*pvar".
 */
#define SET_FOREACH(pvar, set)						\
	for (pvar = SET_BEGIN(set); pvar < SET_LIMIT(set); pvar++)

#define SET_ITEM(set, i)						\
	((SET_BEGIN(set))[i])

/*
 * Provide a count of the items in a set.
 */
#define SET_COUNT(set)							\
	(SET_LIMIT(set) - SET_BEGIN(set))

#endif	/* _SYS_LINKER_SET_H_ */