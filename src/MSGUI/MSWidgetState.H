#ifndef MSWidgetStateHEADER
#define MSWidgetStateHEADER

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////


#include <MSTypes/MSHashTable.H>

#if (__GNUC__ < 3)
class istream;
#else
#include <iostream>
#include <fstream>
using namespace std;
#endif


class MSGUIExport MSWidgetState : public MSHashTable
{
public:
  MSWidgetState (istream &is_);
  ~MSWidgetState(void);
};

#endif
