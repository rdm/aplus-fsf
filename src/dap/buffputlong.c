/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1989-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* contributed by Chuck Ocheret */

/* header file inclusions */
#include <dap/buff.h>

/* external function definitions */
void 
buffputlong(struct buff * p, long int d)
{
  buffstuff(p, (char *) (&d), sizeof(d));
  return;
}
