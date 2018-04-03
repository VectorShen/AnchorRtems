/**
 *  @file
 *
 *  @brief Get Lowest Valid API Index
 *  @ingroup ClassicClassInfo
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

/*
 * This is implemented as a macro. This body is provided to support
 * bindings from non-C based languages.
 */
int rtems_object_id_api_minimum (void);

int rtems_object_id_api_minimum (void)
{
	return OBJECTS_INTERNAL_API;
}