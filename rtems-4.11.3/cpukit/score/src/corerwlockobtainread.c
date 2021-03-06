/**
 * @file 
 * 
 * @brief Obtain RWLock for reading
 * @ingroup ScoreRWLock
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/corerwlockimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/watchdog.h>

void _CORE_RWLock_Obtain_for_reading (CORE_RWLock_Control * the_rwlock,
									Thread_Control * executing,
									Objects_Id id,
									bool wait,
									Watchdog_Interval timeout,
									CORE_RWLock_API_mp_support_callout
									api_rwlock_mp_support)
{
	ISR_lock_Context lock_context;

	/*
	 *  If unlocked, then OK to read.
	 *  If locked for reading and no waiters, then OK to read.
	 *  If any thread is waiting, then we wait.
	 */

	_Thread_queue_Acquire (&the_rwlock->Wait_queue, &lock_context);
	switch (the_rwlock->current_state)
	{
		case CORE_RWLOCK_UNLOCKED:
			the_rwlock->current_state = CORE_RWLOCK_LOCKED_FOR_READING;
			the_rwlock->number_of_readers += 1;
			_Thread_queue_Release (&the_rwlock->Wait_queue, &lock_context);
			executing->Wait.return_code = CORE_RWLOCK_SUCCESSFUL;
			return;

		case CORE_RWLOCK_LOCKED_FOR_READING:
			{
				Thread_Control *waiter;
				waiter = _Thread_queue_First_locked (&the_rwlock->Wait_queue);
				if (!waiter)
				{
					the_rwlock->number_of_readers += 1;
					_Thread_queue_Release (&the_rwlock->Wait_queue,
										 &lock_context);
					executing->Wait.return_code = CORE_RWLOCK_SUCCESSFUL;
					return;
				}
				break;
			}
		case CORE_RWLOCK_LOCKED_FOR_WRITING:
			break;
	}

	/*
	 *  If the thread is not willing to wait, then return immediately.
	 */

	if (!wait)
	{
		_Thread_queue_Release (&the_rwlock->Wait_queue, &lock_context);
		executing->Wait.return_code = CORE_RWLOCK_UNAVAILABLE;
		return;
	}

	/*
	 *  We need to wait to enter this critical section
	 */

	executing->Wait.id = id;
	executing->Wait.option = CORE_RWLOCK_THREAD_WAITING_FOR_READ;
	executing->Wait.return_code = CORE_RWLOCK_SUCCESSFUL;

	_Thread_queue_Enqueue_critical (&the_rwlock->Wait_queue,
									executing,
									STATES_WAITING_FOR_RWLOCK,
									timeout,
									CORE_RWLOCK_TIMEOUT, &lock_context);

	/* return to API level so it can dispatch and we block */
}
