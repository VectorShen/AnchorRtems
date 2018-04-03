/**
 * @file
 *
 * @brief Nfsclient Mount Prot
 *
 * @ingroup rtems-nfsclient
 */

#ifndef _MOUNT_PROT_H_RPCGEN
#define _MOUNT_PROT_H_RPCGEN

#include <rpc/rpc.h>

/**
 *  @defgroup libfs_nfsclient_mount_prot Mount Prot
 *
 *  @ingroup libfs
 */
/**@{*/
#ifdef __cplusplus
extern "C"
{
#endif

#define MNTPATHLEN 1024
#define MNTNAMLEN 255
#define FHSIZE 32

	typedef char fhandle[FHSIZE];

	struct fhstatus
	{
		u_int fhs_status;
		union
		{
			fhandle fhs_fhandle;
		} fhstatus_u;
	};
	typedef struct fhstatus fhstatus;

	typedef char *dirpath;

	typedef char *name;

	typedef struct mountbody *mountlist;

	struct mountbody
	{
		name ml_hostname;
		dirpath ml_directory;
		mountlist ml_next;
	};
	typedef struct mountbody mountbody;

	typedef struct groupnode *groups;

	struct groupnode
	{
		name gr_name;
		groups gr_next;
	};
	typedef struct groupnode groupnode;

	typedef struct exportnode *exports;

	struct exportnode
	{
		dirpath ex_dir;
		groups ex_groups;
		exports ex_next;
	};
	typedef struct exportnode exportnode;

#define MOUNTPROG 100005
#define MOUNTVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define MOUNTPROC_NULL 0
	extern void *mountproc_null_1 (void *, CLIENT *);
	extern void *mountproc_null_1_svc (void *, struct svc_req *);
#define MOUNTPROC_MNT 1
	extern fhstatus *mountproc_mnt_1 (dirpath *, CLIENT *);
	extern fhstatus *mountproc_mnt_1_svc (dirpath *, struct svc_req *);
#define MOUNTPROC_DUMP 2
	extern mountlist *mountproc_dump_1 (void *, CLIENT *);
	extern mountlist *mountproc_dump_1_svc (void *, struct svc_req *);
#define MOUNTPROC_UMNT 3
	extern void *mountproc_umnt_1 (dirpath *, CLIENT *);
	extern void *mountproc_umnt_1_svc (dirpath *, struct svc_req *);
#define MOUNTPROC_UMNTALL 4
	extern void *mountproc_umntall_1 (void *, CLIENT *);
	extern void *mountproc_umntall_1_svc (void *, struct svc_req *);
#define MOUNTPROC_EXPORT 5
	extern exports *mountproc_export_1 (void *, CLIENT *);
	extern exports *mountproc_export_1_svc (void *, struct svc_req *);
#define MOUNTPROC_EXPORTALL 6
	extern exports *mountproc_exportall_1 (void *, CLIENT *);
	extern exports *mountproc_exportall_1_svc (void *, struct svc_req *);
	extern int mountprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else							/* K&R C */
#define MOUNTPROC_NULL 0
	extern void *mountproc_null_1 ();
	extern void *mountproc_null_1_svc ();
#define MOUNTPROC_MNT 1
	extern fhstatus *mountproc_mnt_1 ();
	extern fhstatus *mountproc_mnt_1_svc ();
#define MOUNTPROC_DUMP 2
	extern mountlist *mountproc_dump_1 ();
	extern mountlist *mountproc_dump_1_svc ();
#define MOUNTPROC_UMNT 3
	extern void *mountproc_umnt_1 ();
	extern void *mountproc_umnt_1_svc ();
#define MOUNTPROC_UMNTALL 4
	extern void *mountproc_umntall_1 ();
	extern void *mountproc_umntall_1_svc ();
#define MOUNTPROC_EXPORT 5
	extern exports *mountproc_export_1 ();
	extern exports *mountproc_export_1_svc ();
#define MOUNTPROC_EXPORTALL 6
	extern exports *mountproc_exportall_1 ();
	extern exports *mountproc_exportall_1_svc ();
	extern int mountprog_1_freeresult ();
#endif							/* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
	extern bool_t xdr_fhandle (XDR *, fhandle);
	extern bool_t xdr_fhstatus (XDR *, fhstatus *);
	extern bool_t xdr_dirpath (XDR *, dirpath *);
	extern bool_t xdr_name (XDR *, name *);
	extern bool_t xdr_mountlist (XDR *, mountlist *);
	extern bool_t xdr_mountbody (XDR *, mountbody *);
	extern bool_t xdr_groups (XDR *, groups *);
	extern bool_t xdr_groupnode (XDR *, groupnode *);
	extern bool_t xdr_exports (XDR *, exports *);
	extern bool_t xdr_exportnode (XDR *, exportnode *);

#else							/* K&R C */
	extern bool_t xdr_fhandle ();
	extern bool_t xdr_fhstatus ();
	extern bool_t xdr_dirpath ();
	extern bool_t xdr_name ();
	extern bool_t xdr_mountlist ();
	extern bool_t xdr_mountbody ();
	extern bool_t xdr_groups ();
	extern bool_t xdr_groupnode ();
	extern bool_t xdr_exports ();
	extern bool_t xdr_exportnode ();

#endif							/* K&R C */

#ifdef __cplusplus
}
#endif
/**@}*/
#endif							/* !_MOUNT_PROT_H_RPCGEN */
