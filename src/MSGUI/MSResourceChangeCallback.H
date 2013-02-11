#ifndef MSResourceChangeCallbackHEADER
#define MSResourceChangeCallbackHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSTypes/MSCallback.H>

class MSWidget;
class MSGUIExport MSResourceChangeCallback : public MSCallback
{
protected:
  MSWidget *_widget;

public:
  MSResourceChangeCallback(MSWidget *widget_=0) :_widget(widget_) {}

  MSWidget *widget(void) {return _widget;}

  virtual void process(void) {}
  virtual void resourceChanged(const char *)=0;
};

#endif

