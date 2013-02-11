#ifndef MSSeparatorMenuItemHEADER
#define MSSeparatorMenuItemHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSGUI/MSMenuItem.H>

class MSGUIExport MSSeparatorMenuItem : public MSMenuItem 
{
protected:
  void computeSize(void);
  void init(void);

  virtual void drawSymbol(void);

public:
  MSSeparatorMenuItem(MSMenu *);
  ~MSSeparatorMenuItem(void);
};

#endif
