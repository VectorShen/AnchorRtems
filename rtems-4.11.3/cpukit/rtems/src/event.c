/**
 *  @file
 *
 *  @brief Event Manager Initialization
 *  @ingroup ClassicEvent
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>

void _Event_Manager_initialization (void)
{
	/*
	 *  Register the MP Process Packet routine.
	 */

#if defined(RTEMS_MULTIPROCESSING)
	_MPCI_Register_packet_processor (MP_PACKET_EVENT, _Event_MP_Process_packet);
#endif
}
