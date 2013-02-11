///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSGUI/MSWidgetFreezer.H>
#include <MSGUI/MSWidget.H>

MSWidgetFreezer::MSWidgetFreezer(MSWidget *widget_): _widget(widget_)
{
  if(widget_ != 0){
    _wasFrozen = _widget->frozen(); 
    _widget->freeze(); 
  }
}

MSWidgetFreezer::~MSWidgetFreezer(void) 
{ if (_widget != 0  && _wasFrozen == MSFalse) _widget->unfreeze(); }

