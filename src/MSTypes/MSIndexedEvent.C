///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSTypes/MSIndexedEvent.H>

MSIndexedEvent::~MSIndexedEvent(void) {}

const MSSymbol& MSIndexedEvent::symbol(void) 
{
  static MSSymbol sym ("MSIndexedEvent");
  return sym;
}

