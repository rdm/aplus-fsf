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
#include <dap/fds.h>

/* external function definitions */
fd_set *
fdsdup_r(fd_set * p)
{
  fd_set *p2 = (fd_set *) balloc_r(fds_sizeof);

  fdscopy(p, p2);

  return p2;
}
