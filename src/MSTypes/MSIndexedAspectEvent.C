///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSTypes/MSIndexedAspectEvent.H>

MSIndexedAspectEvent::~MSIndexedAspectEvent(void)
{}

const MSSymbol& MSIndexedAspectEvent::symbol(void) 
{
  static MSSymbol symbol("MSIndexedAspectEvent");
  return symbol;
}
