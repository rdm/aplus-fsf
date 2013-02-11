///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSTypes/MSStack.H>

void MSStack::push(MSNodeItem *n_)
{
  n_->insert(next());
}

MSNodeItem *MSStack::pop(void)
{
  if (next()==this) return 0;
  else
   {
     MSNodeItem *r=next();
     r->remove();
     return r;
   }
}

