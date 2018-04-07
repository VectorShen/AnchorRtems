/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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
 */

#include <machine/rtems-bsd-kernel-space.h>
#include <machine/rtems-bsd-muteximpl.h>

#include <rtems/bsd/sys/param.h>
#include <rtems/bsd/sys/types.h>
#include <sys/systm.h>
#include <rtems/bsd/sys/lock.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/conf.h>

static void assert_mtx(struct lock_object *lock, int what);
static void lock_mtx(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_mtx(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_mtx(struct lock_object *lock);

/*
 * Lock classes for sleep and spin mutexes.
 */
struct lock_class lock_class_mtx_sleep = {
  .lc_name = "sleep mutex",
  .lc_flags = LC_SLEEPLOCK | LC_RECURSABLE,
  .lc_assert = assert_mtx,
#ifdef DDB
  .lc_ddb_show = db_show_mtx,
#endif
  .lc_lock = lock_mtx,
  .lc_unlock = unlock_mtx,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_mtx,
#endif
};

struct lock_class lock_class_mtx_spin = {
  .lc_name = "spin mutex",
  .lc_flags = LC_SPINLOCK | LC_RECURSABLE,
  .lc_assert = assert_mtx,
#ifdef DDB
  .lc_ddb_show = db_show_mtx,
#endif
  .lc_lock = lock_mtx,
  .lc_unlock = unlock_mtx,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_mtx,
#endif
};

struct mtx Giant;

void
assert_mtx(struct lock_object *lock, int what)
{
  mtx_assert((struct mtx *)lock, what);
}

void
lock_mtx(struct lock_object *lock, int how)
{
	mtx_lock((struct mtx *)lock);
}

int
unlock_mtx(struct lock_object *lock)
{
	mtx_unlock((struct mtx *)lock);

	return (0);
}


#ifdef KDTRACE_HOOKS
int
owner_mtx(struct lock_object *lock, struct thread **owner)
{
  struct mtx *m = (struct mtx *)lock;

  *owner = mtx_owner(m);
  return (mtx_unowned(m) == 0);
}
#endif

void
mtx_init(struct mtx *m, const char *name, const char *type, int opts)
{
	struct lock_class *class;
	int flags;

	/* Determine lock class and lock flags. */
	if (opts & MTX_SPIN)
		class = &lock_class_mtx_spin;
	else
		class = &lock_class_mtx_sleep;
	flags = 0;
	if (opts & MTX_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_mutex_init(&m->lock_object, &m->mutex, class, name, type,
	    flags);
}

void
_mtx_lock_flags(struct mtx *m, int opts, const char *file, int line)
{
	rtems_bsd_mutex_lock(&m->lock_object, &m->mutex);
}

int
_mtx_trylock(struct mtx *m, int opts, const char *file, int line)
{
	return (rtems_bsd_mutex_trylock(&m->lock_object, &m->mutex));
}

void
_mtx_unlock_flags(struct mtx *m, int opts, const char *file, int line)
{
	rtems_bsd_mutex_unlock(&m->mutex);
}

/*
 * The backing function for the INVARIANTS-enabled mtx_assert()
 */
#ifdef INVARIANT_SUPPORT
void
_mtx_assert(struct mtx *m, int what, const char *file, int line)
{

  if (panicstr != NULL || dumping)
    return;
  switch (what) {
  case MA_OWNED:
  case MA_OWNED | MA_RECURSED:
  case MA_OWNED | MA_NOTRECURSED:
    if (!mtx_owned(m))
      panic("mutex %s not owned at %s:%d",
          m->lock_object.lo_name, file, line);
    if (mtx_recursed(m)) {
      if ((what & MA_NOTRECURSED) != 0)
        panic("mutex %s recursed at %s:%d",
            m->lock_object.lo_name, file, line);
    } else if ((what & MA_RECURSED) != 0) {
      panic("mutex %s unrecursed at %s:%d",
          m->lock_object.lo_name, file, line);
    }
    break;
  case MA_NOTOWNED:
    if (mtx_owned(m))
      panic("mutex %s owned at %s:%d",
          m->lock_object.lo_name, file, line);
    break;
  default:
    panic("unknown mtx_assert at %s:%d", file, line);
  }
}
#endif

int mtx_owned(struct mtx *m)
{
	return (rtems_bsd_mutex_owned(&m->mutex));
}

int mtx_recursed(struct mtx *m)
{
	return (rtems_bsd_mutex_recursed(&m->mutex));
}

void
mtx_sysinit(void *arg)
{
	struct mtx_args *margs = arg;

	mtx_init(margs->ma_mtx, margs->ma_desc, NULL, margs->ma_opts);
}

void
mtx_destroy(struct mtx *m)
{

	rtems_bsd_mutex_destroy(&m->lock_object, &m->mutex);
}

void
mutex_init(void)
{
	mtx_init(&Giant, "Giant", NULL, MTX_DEF | MTX_RECURSE);
	mtx_lock(&Giant);
}
