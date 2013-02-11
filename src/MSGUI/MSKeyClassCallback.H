#ifndef MSKeyClassCallbackHEADER
#define MSKeyClassCallbackHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1998-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSGUI/MSKeyCallback.H>


template <class MethodClass> 
class MSKeyClassCallback : public MSKeyCallback
{
  typedef void (MethodClass::*Method)(void);
private:
  Method       _method;
public:
  MSKeyClassCallback(Method method_) :
      _method(method_) {}
  
  ~MSKeyClassCallback(void) {}

  MSBoolean process(MSWidget* widget_,const MSKeyPress&)
    {   
      (((MethodClass*)widget_)->*_method)(); 
      return MSTrue;
    } 
};

#endif
