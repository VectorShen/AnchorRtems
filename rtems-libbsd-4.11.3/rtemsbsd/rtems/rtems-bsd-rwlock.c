/**
 * @file
 *
 * @ingroup rtems_bsd_rtems
 *
 * @brief TODO.
 */

/*
 * Copyright (c) 2011 OPTI Medical.  All rights reserved.
 *
 *  OPTI Medical
 *  235 Hembree Park Drive
 *  Roswell, GA 30076
 *  USA
 *  <kevin.kirspel@optimedical.com>
 *
 * Copyright (c) 2013-2015 embedded brains GmbH.  All rights reserved.
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
#include <sys/rwlock.h>

#ifndef INVARIANTS
#define _rw_assert(rw, what, file, line)
#endif

static void assert_rw(struct lock_object *lock, int what);
static void lock_rw(struct lock_object *lock, int how);
#ifdef KDTRACE_HOOKS
static int  owner_rw(struct lock_object *lock, struct thread **owner);
#endif
static int  unlock_rw(struct lock_object *lock);

struct lock_class lock_class_rw = {
  .lc_name = "rw",
  .lc_flags = LC_SLEEPLOCK | LC_RECURSABLE | LC_UPGRADABLE,
  .lc_assert = assert_rw,
#ifdef DDB
  .lc_ddb_show = db_show_rwlock,
#endif
  .lc_lock = lock_rw,
  .lc_unlock = unlock_rw,
#ifdef KDTRACE_HOOKS
  .lc_owner = owner_rw,
#endif
};

#define rw_wowner(rw) ((rw)->mutex.owner)

#define rw_recursed(rw) ((rw)->mutex.nest_level != 0)

void
assert_rw(struct lock_object *lock, int what)
{
  rw_assert((struct rwlock *)lock, what);
}

void
lock_rw(struct lock_object *lock, int how)
{
  rw_wlock((struct rwlock *)lock);
}

int
unlock_rw(struct lock_object *lock)
{
  rw_unlock((struct rwlock *)lock);

  return (0);
}

#ifdef KDTRACE_HOOKS
int
owner_rw(struct lock_object *lock, struct thread **owner)
{
  struct rwlock *rw = (struct rwlock *)lock;
  uintptr_t x = rw->rw_lock;

  *owner = rw_wowner(rw);
  return ((x & RW_LOCK_READ) != 0 ?  (RW_READERS(x) != 0) :
      (*owner != NULL));
}
#endif

void
rw_init_flags(struct rwlock *rw, const char *name, int opts)
{
	int flags;

	flags = LO_UPGRADABLE;
	if (opts & RW_RECURSE)
		flags |= LO_RECURSABLE;

	rtems_bsd_mutex_init(&rw->lock_object, &rw->mutex, &lock_class_rw,
	    name, NULL, flags);
}

void
rw_destroy(struct rwlock *rw)
{

	rtems_bsd_mutex_destroy(&rw->lock_object, &rw->mutex);
}

void
rw_sysinit(void *arg)
{
  struct rw_args *args = arg;

  rw_init(args->ra_rw, args->ra_desc);
}

void
rw_sysinit_flags(void *arg)
{
  struct rw_args_flags *args = arg;

  rw_init_flags(args->ra_rw, args->ra_desc, args->ra_flags);
}

int
rw_wowned(struct rwlock *rw)
{
	return (rtems_bsd_mutex_owned(&rw->mutex));
}

void
_rw_wlock(struct rwlock *rw, const char *file, int line)
{
	rtems_bsd_mutex_lock(&rw->lock_object, &rw->mutex);
}

int
_rw_try_wlock(struct rwlock *rw, const char *file, int line)
{
	return (rtems_bsd_mutex_trylock(&rw->lock_object, &rw->mutex));
}

void
_rw_wunlock(struct rwlock *rw, const char *file, int line)
{
	rtems_bsd_mutex_unlock(&rw->mutex);
}

void
_rw_rlock(struct rwlock *rw, const char *file, int line)
{
	rtems_bsd_mutex_lock(&rw->lock_object, &rw->mutex);
}

int
_rw_try_rlock(struct rwlock *rw, const char *file, int line)
{
	return (rtems_bsd_mutex_trylock(&rw->lock_object, &rw->mutex));
}

void
_rw_runlock(struct rwlock *rw, const char *file, int line)
{
	rtems_bsd_mutex_unlock(&rw->mutex);
}

int
_rw_try_upgrade(struct rwlock *rw, const char *file, int line)
{
	return (1);
}

void
_rw_downgrade(struct rwlock *rw, const char *file, int line)
{
	/* Nothing to do */
}

#ifdef INVARIANT_SUPPORT
#ifndef INVARIANTS
#undef _rw_assert
#endif

/*
 * In the non-WITNESS case, rw_assert() can only detect that at least
 * *some* thread owns an rlock, but it cannot guarantee that *this*
 * thread owns an rlock.
 */
void
_rw_assert(struct rwlock *rw, int what, const char *file, int line)
{

  if (panicstr != NULL)
    return;
  switch (what) {
  case RA_LOCKED:
  case RA_LOCKED | RA_RECURSED:
  case RA_LOCKED | RA_NOTRECURSED:
  case RA_RLOCKED:
#ifndef __rtems__
#ifdef WITNESS
    witness_assert(&rw->lock_object, what, file, line);
#else
    /*
     * If some other thread has a write lock or we have one
     * and are asserting a read lock, fail.  Also, if no one
     * has a lock at all, fail.
     */
    if (rw->rw_lock == RW_UNLOCKED ||
        (!(rw->rw_lock & RW_LOCK_READ) && (what == RA_RLOCKED ||
        rw_wowner(rw) != curthread)))
      panic("Lock %s not %slocked @ %s:%d\n",
          rw->lock_object.lo_name, (what == RA_RLOCKED) ?
          "read " : "", file, line);

    if (!(rw->rw_lock & RW_LOCK_READ)) {
      if (rw_recursed(rw)) {
        if (what & RA_NOTRECURSED)
          panic("Lock %s recursed @ %s:%d\n",
              rw->lock_object.lo_name, file,
              line);
      } else if (what & RA_RECURSED)
        panic("Lock %s not recursed @ %s:%d\n",
            rw->lock_object.lo_name, file, line);
    }
#endif
    break;
#else /* __rtems__ */
    /* FALLTHROUGH */
#endif /* __rtems__ */
  case RA_WLOCKED:
  case RA_WLOCKED | RA_RECURSED:
  case RA_WLOCKED | RA_NOTRECURSED:
    if (rw_wowner(rw) != _Thread_Get_executing())
      panic("Lock %s not exclusively locked @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
    if (rw_recursed(rw)) {
      if (what & RA_NOTRECURSED)
        panic("Lock %s recursed @ %s:%d\n",
            rw->lock_object.lo_name, file, line);
    } else if (what & RA_RECURSED)
      panic("Lock %s not recursed @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
    break;
  case RA_UNLOCKED:
#ifdef WITNESS
    witness_assert(&rw->lock_object, what, file, line);
#else
    /*
     * If we hold a write lock fail.  We can't reliably check
     * to see if we hold a read lock or not.
     */
    if (rw_wowner(rw) == _Thread_Get_executing())
      panic("Lock %s exclusively locked @ %s:%d\n",
          rw->lock_object.lo_name, file, line);
#endif
    break;
  default:
    panic("Unknown rw lock assertion: %d @ %s:%d", what, file,
        line);
  }
}
#endif /* INVARIANT_SUPPORT */
