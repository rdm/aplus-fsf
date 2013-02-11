#ifndef MSHLayoutHEADER
#define MSHLayoutHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSGUI/MSLayout.H>

class MSGUIExport MSHLayout : public MSLayout
{
public:
  MSHLayout(MSWidget *,const char *title_=0);
  MSHLayout(MSWidget *,const MSStringVector&);
  ~MSHLayout(void);
private:
  void init(void);
};

#endif




