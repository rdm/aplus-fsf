/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 1990-2001 Morgan Stanley Dean Witter & Co. All rights reserved.*/
/* See .../src/LICENSE for terms of distribution.                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*
 * This file replaces b.c to remove all references to the "buddy system"
 * in m.c.  ma(), mab(), and mf() are replaced here with covers to
 * malloc() and free().  mi(), which initializes the buddy system, is
 * replaced with a no-op.
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <a/k.h>

char * mab(unsigned long w)
{
  char *p; 
  unsigned long *pl; 
 
  if(0==w) 
    { 
      printf("\343 ma(): Warning: attempt to allocate 0 bytes\n");w=1; 
    } 
 
  if ((pl = (unsigned long *)malloc(w+2*sizeof(*pl))) == (unsigned long *)(0)) 
    { 
      printf("\343 ma(): malloc failure: errno=%d",errno); 
      p=(char *)pl; 
    } 
  else 
    { 
      pl[0]=w; 
      pl[1]=w; 
      p=(char *)&(pl[2]); 
    } 
  return p; 
  
}

long *ma(unsigned long w)
/* w - number of words required */
{
  return (long *)(mab(w*sizeof(long)));
}

void mf(long *p)
{
 
  if(p && (*(p-1))==(*(p-2))) 
    free (p-2); 
  else 
    { 
      printf("!! Not an mab pointer !!\n"); 
      free(p); 
    } 
}

void mi(void) {}

