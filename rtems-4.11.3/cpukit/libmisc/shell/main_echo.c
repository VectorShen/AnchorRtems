/*  $NetBSD: echo.c,v 1.12 2005/02/06 04:43:43 perry Exp $  */

/*-
 * Copyright (c) 1991, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)echo.c  8.1 (Berkeley) 5/31/93
 */

/*
 * Echo command.
 *
 * echo is steeped in tradition - several of them!
 * netbsd has supported 'echo [-n | -e] args' in spite of -e not being
 * documented anywhere.
 * Posix requires that -n be supported, output from strings containing
 * \ is implementation defined
 * The Single Unix Spec requires that \ escapes be treated as if -e
 * were set, but that -n not be treated as an option.
 * (ksh supports 'echo [-eEn] args', but not -- so that it is actually
 * impossible to actually output '-n')
 *
 * It is suggested that 'printf "%b" "string"' be used to get \ sequences
 * expanded.  printf is now a builtin of netbsd's sh and csh.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_echo (int argc, char *argv[])
{
	char **ap;
	char *p;
	char c;
	int count;
	int nflag = 0;
	int eflag = 0;

	ap = argv;
	if (argc)
		ap++;

	if ((p = *ap) != NULL)
	{
		if (!strcmp (p, "-n"))
		{
			nflag = 1;
			ap++;
		}
		else if (!strcmp (p, "-e"))
		{
			eflag = 1;
			ap++;
		}
	}

	while ((p = *ap++) != NULL)
	{
		while ((c = *p++) != '\0')
		{
			if (c == '\\' && eflag)
			{
				switch (*p++)
				{
					case 'a':
						c = '\a';
						break;	/* bell */
					case 'b':
						c = '\b';
						break;
					case 'c':
						return 0;	/* exit */
					case 'e':
						c = 033;
						break;	/* escape */
					case 'f':
						c = '\f';
						break;
					case 'n':
						c = '\n';
						break;
					case 'r':
						c = '\r';
						break;
					case 't':
						c = '\t';
						break;
					case 'v':
						c = '\v';
						break;
					case '\\':
						break;	/* c = '\\' */
					case '0':
						c = 0;
						count = 3;
						while (--count >= 0 && (unsigned)(*p - '0') < 8)
							c = (c << 3) + (*p++ - '0');
						break;
					default:
						/* Output the '/' and char following */
						p--;
						break;
				}
			}
			putchar (c);
		}
		if (*ap)
			putchar (' ');
	}
	if (!nflag)
		putchar ('\n');
	return 0;
}

rtems_shell_cmd_t rtems_shell_ECHO_Command =
{
	"echo",						/* name */
	"echo [args]",				/* usage */
	"misc",						/* topic */
	rtems_shell_main_echo,		/* command */
	NULL,						/* alias */
	NULL						/* next */
};
