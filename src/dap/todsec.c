/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1989-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/* contributed by Daniel F. Fisher */

/* header file inclusions */
#include <sys/types.h>
#include <dap/tod.h>

/* external function definitions */
time_t 
todsec(void)
{
  return (time_t) ((tod())->tv_sec);
}
