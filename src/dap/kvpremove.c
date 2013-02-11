/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1989-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* contributed by Daniel F. Fisher */

/* header file inclusions */
#include <dap/node.h>
#include <dap/kvp.h>

void 
kvpremove(struct node * np)
{
  noderemove(np);
  kvpfree(KVPAT(np));
  nodefree(np);
  return;
}