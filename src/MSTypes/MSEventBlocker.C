///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSTypes/MSEventBlocker.H>
#include <MSTypes/MSEventSender.H>

MSEventBlocker::MSEventBlocker(MSEventSender *sender_,MSBoolean sendEvent_) :
_sender(sender_),_sendEvent(sendEvent_) 
{ if (_sender!=0) _sender->blockEvents(); }

MSEventBlocker::~MSEventBlocker(void) 
{ if (_sender!=0) _sender->unblockEvents(_sendEvent); }



