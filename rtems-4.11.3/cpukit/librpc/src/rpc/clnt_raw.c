/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)clnt_raw.c 1.22 87/08/11 Copyr 1984 Sun Micro";*/
/*static char *sccsid = "from: @(#)clnt_raw.c	2.2 88/08/01 4.0 RPCSRC";*/
static char *rcsid =
	"$FreeBSD: src/lib/libc/rpc/clnt_raw.c,v 1.10 1999/08/28 00:00:36 peter Exp $";
#endif

/*
 * clnt_raw.c
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * Memory based rpc for simple testing and timing.
 * Interface to create an rpc client and server in the same process.
 * This lets us similate rpc and get round trip overhead, without
 * any interference from the kernel.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include <stdlib.h>
#include <stdio.h>

#define MCALL_MSG_SIZE 24

/*
 * This is the "network" we will be moving stuff over.
 */
struct clnt_raw_private
{
	CLIENT client_object;
	XDR xdr_stream;
	char _raw_buf[UDPMSGSIZE];
	union
	{
		struct rpc_msg mashl_rpcmsg;
		char mashl_callmsg[MCALL_MSG_SIZE];
	} u;
	u_int mcnt;
};
#define clntraw_private (rtems_rpc_task_variables->clnt_raw_private)

static enum clnt_stat clntraw_call (CLIENT * h, rpcproc_t proc, xdrproc_t xargs,
									void *argsp, xdrproc_t xresults,
									void *resultsp, struct timeval timeout);
static void clntraw_abort (void);
static void clntraw_geterr (CLIENT * h, struct rpc_err *);
static bool_t clntraw_freeres (CLIENT *, xdrproc_t, void *);
static bool_t clntraw_control (CLIENT *, int, char *);
static void clntraw_destroy (CLIENT *);

static struct clnt_ops client_ops =
{
	clntraw_call,
	clntraw_abort,
	clntraw_geterr,
	clntraw_freeres,
	clntraw_destroy,
	clntraw_control
};

/*
 * Create a client handle for memory based rpc.
 */
CLIENT *clntraw_create (u_long prog, u_long vers)
{
	struct clnt_raw_private *clp = clntraw_private;
	struct rpc_msg call_msg;
	XDR *xdrs = &clp->xdr_stream;
	CLIENT *client = &clp->client_object;

	if (clp == 0)
	{
		clp = (struct clnt_raw_private *)calloc (1, sizeof (*clp));
		if (clp == 0)
			return (0);
		clntraw_private = clp;
	}
	/*
	 * pre-serialize the static part of the call msg and stash it away
	 */
	call_msg.rm_direction = CALL;
	call_msg.rm_call.cb_rpcvers = RPC_MSG_VERSION;
	call_msg.rm_call.cb_prog = prog;
	call_msg.rm_call.cb_vers = vers;
	xdrmem_create (xdrs, clp->u.mashl_callmsg, MCALL_MSG_SIZE, XDR_ENCODE);
	if (!xdr_callhdr (xdrs, &call_msg))
	{
		perror ("clnt_raw.c - Fatal header serialization error.");
	}
	clp->mcnt = XDR_GETPOS (xdrs);
	XDR_DESTROY (xdrs);

	/*
	 * Set xdrmem for client/server shared buffer
	 */
	xdrmem_create (xdrs, clp->_raw_buf, UDPMSGSIZE, XDR_FREE);

	/*
	 * create client handle
	 */
	client->cl_ops = &client_ops;
	client->cl_auth = authnone_create ();
	return (client);
}

static enum clnt_stat
clntraw_call (CLIENT * h,
			rpcproc_t proc,
			xdrproc_t xargs,
			void *argsp,
			xdrproc_t xresults, void *resultsp, struct timeval timeout)
{
	struct clnt_raw_private *clp = clntraw_private;
	XDR *xdrs = &clp->xdr_stream;
	struct rpc_msg msg;
	enum clnt_stat status;
	struct rpc_err error;

	if (clp == 0)
		return (RPC_FAILED);
  call_again:
	/*
	 * send request
	 */
	xdrs->x_op = XDR_ENCODE;
	XDR_SETPOS (xdrs, 0);
	clp->u.mashl_rpcmsg.rm_xid++;
	if ((!XDR_PUTBYTES (xdrs, clp->u.mashl_callmsg, clp->mcnt)) ||
		(!XDR_PUTLONG (xdrs, (long *)&proc)) ||
		(!AUTH_MARSHALL (h->cl_auth, xdrs)) || (!(*xargs) (xdrs, argsp)))
	{
		return (RPC_CANTENCODEARGS);
	}
	(void)XDR_GETPOS (xdrs);	/* called just to cause overhead */

	/*
	 * We have to call server input routine here because this is
	 * all going on in one process. Yuk.
	 */
	svc_getreq (1);

	/*
	 * get results
	 */
	xdrs->x_op = XDR_DECODE;
	XDR_SETPOS (xdrs, 0);
	msg.acpted_rply.ar_verf = _null_auth;
	msg.acpted_rply.ar_results.where = resultsp;
	msg.acpted_rply.ar_results.proc = xresults;
	if (!xdr_replymsg (xdrs, &msg))
		return (RPC_CANTDECODERES);
	_seterr_reply (&msg, &error);
	status = error.re_status;

	if (status == RPC_SUCCESS)
	{
		if (!AUTH_VALIDATE (h->cl_auth, &msg.acpted_rply.ar_verf))
		{
			status = RPC_AUTHERROR;
		}
	}							/* end successful completion */
	else
	{
		if (AUTH_REFRESH (h->cl_auth))
			goto call_again;
	}							/* end of unsuccessful completion */

	if (status == RPC_SUCCESS)
	{
		if (!AUTH_VALIDATE (h->cl_auth, &msg.acpted_rply.ar_verf))
		{
			status = RPC_AUTHERROR;
		}
		if (msg.acpted_rply.ar_verf.oa_base != NULL)
		{
			xdrs->x_op = XDR_FREE;
			(void)xdr_opaque_auth (xdrs, &(msg.acpted_rply.ar_verf));
		}
	}

	return (status);
}

static void clntraw_geterr (CLIENT * cl, struct rpc_err *err)
{
}

static bool_t clntraw_freeres (CLIENT * cl, xdrproc_t xdr_res, void *res_ptr)
{
	struct clnt_raw_private *clp = clntraw_private;
	XDR *xdrs = &clp->xdr_stream;
	bool_t rval;

	if (clp == 0)
	{
		rval = (bool_t) RPC_FAILED;
		return (rval);
	}
	xdrs->x_op = XDR_FREE;
	return ((*xdr_res) (xdrs, res_ptr));
}

static void clntraw_abort (void)
{
}

static bool_t clntraw_control (CLIENT * cl, int request, char *info)
{
	return (FALSE);
}

static void clntraw_destroy (CLIENT * cl)
{
}
