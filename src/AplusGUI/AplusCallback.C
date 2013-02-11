///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved.
// See .../src/LICENSE for terms of distribution.
//
//
///////////////////////////////////////////////////////////////////////////////
#include <AplusGUI/AplusCallback.H>

AplusCallback::AplusCallback(AClientData *ac_)
{ _ac = ac_; }
AplusCallback::~AplusCallback(void)
{ delete _ac;}

void AplusCallback::process(void)
{
//  MSDeleteQueue::allowDelete(MSFalse);
  A r=af4(ac()->function(),(I)ac()->data(),0,0,0,ac()->aplusVar());
  if (r==0) showError(qs);
  else dc(r);
//  MSDeleteQueue::allowDelete(MSTrue);
}
