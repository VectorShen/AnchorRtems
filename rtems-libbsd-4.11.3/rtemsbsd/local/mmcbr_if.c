#include <machine/rtems-bsd-kernel-space.h>

/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   freebsd-org/sys/dev/mmc/mmcbr_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <rtems/bsd/sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/types.h>
#include <sys/bus.h>
#include <dev/mmc/bridge.h>
#include <dev/mmc/mmcreg.h>
#include <rtems/bsd/local/mmcbr_if.h>

struct kobj_method mmcbr_update_ios_method_default = {
	&mmcbr_update_ios_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc mmcbr_update_ios_desc = {
	0, &mmcbr_update_ios_method_default
};

struct kobj_method mmcbr_request_method_default = {
	&mmcbr_request_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc mmcbr_request_desc = {
	0, &mmcbr_request_method_default
};

struct kobj_method mmcbr_get_ro_method_default = {
	&mmcbr_get_ro_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc mmcbr_get_ro_desc = {
	0, &mmcbr_get_ro_method_default
};

struct kobj_method mmcbr_acquire_host_method_default = {
	&mmcbr_acquire_host_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc mmcbr_acquire_host_desc = {
	0, &mmcbr_acquire_host_method_default
};

struct kobj_method mmcbr_release_host_method_default = {
	&mmcbr_release_host_desc, (kobjop_t) kobj_error_method
};

struct kobjop_desc mmcbr_release_host_desc = {
	0, &mmcbr_release_host_method_default
};

