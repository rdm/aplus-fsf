/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1989-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* contributed by Daniel F. Fisher */

/* header file inclusions */
#include <dap/fds.h>
#include <dap/chan.h>

/* external function definitions */
int 
chanisdsbl(struct chan * p)
{
  if ((p != (struct chan *) (0))
      && (fdsisset(p->fds, p->fd))) {
    return 0;
  }
  return 1;
}
