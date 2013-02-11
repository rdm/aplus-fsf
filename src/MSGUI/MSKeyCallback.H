#ifndef MSKeyCallbackHEADER
#define MSKeyCallbackHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////
#include <MSGUI/MSKeyPress.H>

class MSWidget;

class MSGUIExport MSKeyCallback
{
public:
  MSKeyCallback();
  virtual ~MSKeyCallback();
  virtual MSBoolean process(MSWidget*,const MSKeyPress&)=0 ;
};

#endif
