/**
 * @file rtems/score/cpuopts.h
 */

/* target cpu dependent options file */
/* automatically generated -- DO NOT EDIT!! */
#ifndef _RTEMS_SCORE_CPUOPTS_H
#define _RTEMS_SCORE_CPUOPTS_H

/* if RTEMS_DEBUG is enabled */
/* #undef RTEMS_DEBUG */

/* if multiprocessing is enabled */
/* #undef RTEMS_MULTIPROCESSING */

/* if using newlib */
#define RTEMS_NEWLIB 1

/* if posix api is supported */
#define RTEMS_POSIX_API 1

/* if SMP is enabled */
/* #undef RTEMS_SMP */

/* PARAVIRT is enabled */
/* #undef RTEMS_PARAVIRT */

/* if profiling is enabled */
/* #undef RTEMS_PROFILING */

/* if networking is enabled */
#define RTEMS_NETWORKING 1

/* if driver manager api is supported */
/* #undef RTEMS_DRVMGR_STARTUP */

/* RTEMS version string */
#define RTEMS_VERSION "4.11.3"

/* indicate if <sys/cpuset.h> is present in toolset */
#define __RTEMS_HAVE_SYS_CPUSET_H__ 1

/* indicate if <signal.h> in toolset has sigaltstack() */
#define __RTEMS_HAVE_DECL_SIGALTSTACK__ 1

/* disable inlining _Thread_Enable_dispatch */
/* #undef __RTEMS_DO_NOT_INLINE_THREAD_ENABLE_DISPATCH__ */

/* disable inlining _Thread_Enable_dispatch */
/* #undef __RTEMS_DO_NOT_INLINE_CORE_MUTEX_SEIZE__ */

/* disable strict order mutex */
/* #undef __RTEMS_STRICT_ORDER_MUTEX__ */

/* Define to 1 if ada/gnat bindings are built-in */
/* #undef __RTEMS_ADA__ */

/* major version portion of an RTEMS release */
#define __RTEMS_MAJOR__ 4

/* minor version portion of an RTEMS release */
#define __RTEMS_MINOR__ 11

/* revision version portion of an RTEMS release */
#define __RTEMS_REVISION__ 3

#endif /* _RTEMS_SCORE_CPUOPTS_H */
