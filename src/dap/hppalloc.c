/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1989-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* contributed by Daniel F. Fisher */

/* header file inclusions */
#include <dap/balloc.h>
#include <dap/hpp.h>

/* external function definitions */
struct hpp *
hppalloc(char *host, unsigned short port)
/* char *host; */
/* unsigned short port; */
{
  struct hpp *p = (struct hpp *) balloc(sizeof(*p));

  p->host = bstring(host);
  p->port = port;

  return p;
}
