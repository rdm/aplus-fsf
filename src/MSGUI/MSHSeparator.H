#ifndef MSHSeparatorHEADER
#define MSHSeparatorHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSGUI/MSSeparator.H>

class MSGUIExport MSHSeparator : public MSSeparator
{
public:
   MSHSeparator(MSWidget *owner_);
   virtual void redraw(void);
protected:
   virtual void computeSize(void);
};

#endif
