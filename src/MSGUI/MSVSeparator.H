#ifndef MSVSeparatorHEADER
#define MSVSeparatorHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSGUI/MSSeparator.H>

class MSGUIExport MSVSeparator : public MSSeparator
{
public:
   MSVSeparator(MSWidget *owner_);
   virtual void redraw(void);
protected:
   virtual void computeSize(void);
};

#endif
