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
#include <dap/kvp.h>

/* external function definitions */
void 
kvpfree(struct kvp * p)
{
  if (p != (struct kvp *) (0)) {
    bfree((char *) p);
  }
  return;
}
